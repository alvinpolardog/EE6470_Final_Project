#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"


#define SIGNAL_LENGTH 2000
#define NUM_OF_CORE 2
#define DOUBLE_CHANNEL_WIDTH 6

// Gauss Filter ACC
static volatile char * const DETECTOR0_START_ADDR = (char * const)0x73000000;
static volatile char * const DETECTOR0_READ_ADDR = (char * const)0x74000000;
static volatile char * const DETECTOR1_START_ADDR = (char * const)0x75000000;
static volatile char * const DETECTOR1_READ_ADDR = (char * const)0x76000000;


// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = true;
FILE *stim_file;
FILE *resp_file;

unsigned char  input_signal[SIGNAL_LENGTH+DOUBLE_CHANNEL_WIDTH] = {0};
unsigned char  output_signal[SIGNAL_LENGTH+DOUBLE_CHANNEL_WIDTH] = {0};


int sem_init (uint32_t *__sem, uint32_t count) __THROW
{
  *__sem=count;
  return 0;
}


int sem_wait (uint32_t *__sem) __THROW
{
  uint32_t value, success; //RV32A
  __asm__ __volatile__("\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     beqz %[value],L%=                   # if zero, try again\n\t\
     addi %[value],%[value],-1           # value --\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
    : [value] "=r"(value), [success]"=r"(success)
    : [__sem] "r"(__sem)
    : "memory");
  return 0;
}

int sem_post (uint32_t *__sem) __THROW
{
  uint32_t value, success; //RV32A
  __asm__ __volatile__("\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     addi %[value],%[value], 1           # value ++\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
    : [value] "=r"(value), [success]"=r"(success)
    : [__sem] "r"(__sem)
    : "memory");
  return 0;
}

void open_input_file(char* input_file_name){  
  stim_file = fopen(input_file_name,"r");
  if(NULL == stim_file){
    printf("could not open input file\n");
    exit(0);
  }
}

int read_value_from_input(bool * eof){
  int value;

  if(!*eof){
      fscanf (stim_file, "%d", &value);
      *eof = feof(stim_file);
  }
  return value;
}

// Close input stream
void close_input_file(){
  fclose(stim_file);
}

void open_output_file( char* output_file_name ){
  // put output file in appropriate directory in bdw_work
  resp_file = fopen( output_file_name,"w" );
  if( resp_file == NULL){
      printf("could not open output file\n");
      exit( 0 );
  }
}

void write_value_to_output(unsigned char value ){
  fprintf(resp_file, "%d\n", (int)value); 
}

void close_output_file(){
  fclose(resp_file);
}

void write_data_to_ACC(volatile char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(buffer);
    *DMA_DST_ADDR = (uint32_t)(ADDR);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;

  }else{
    // Directly Send
    // memcpy(ADDR, buffer, sizeof(unsigned char)*len);
  }
}
void read_data_from_ACC(volatile char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(ADDR);
    *DMA_DST_ADDR = (uint32_t)(buffer);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Read
    // memcpy(buffer, ADDR, sizeof(unsigned char)*len);
  }
}

union word {
  unsigned int sint;
  unsigned int uint;
  unsigned char uc[4];
};

void print_bin(char value)
{
    for (int i = sizeof(char) * 8; i != -1; i--)
        printf("%d", (value & (1 << i)) >> i );
    putc('\n', stdout);
}

uint32_t dma_lock;
uint32_t print_lock;
bool done_reading = false;
bool done_cal_1 =false;

//int main(unsigned hart_id) {
int main(unsigned hart_id) {
  

  if (hart_id == 0){
    printf("======================================\n");
    printf("\t  Reading Stimulus\n");
    printf("======================================\n");
    open_input_file("stimulus.bin");
    bool eof = false;
    int i = 0;
    while(eof == false){
      input_signal[i++] = read_value_from_input(&eof);
      // printf("%d: %d \n",i,input_signal[i-1]);

    }
    close_input_file();
    sem_init(&print_lock, 1);
    sem_init(&dma_lock, 1);
    done_reading = true;
  }

  while(done_reading==false);
  //while(done_cal_1==false && hart_id ==0);
  
  sem_wait(&print_lock);
  printf("======================================\n");
  printf("\t  Starting Spike Detection of Core #%d\n", hart_id);
  printf("======================================\n");
  sem_post(&print_lock);

  unsigned char  result_signal[10] = {0};
  unsigned char * start_of_section;
  union word return_buffer;
  char char_buffer [8];

  int num_of_term = SIGNAL_LENGTH / NUM_OF_CORE;
  int hart_start = hart_id*num_of_term;
 
  int idx = hart_start+2;
  int result = 0;

  if(hart_id == 0){
    while(idx<hart_start+num_of_term){
      start_of_section = &input_signal[idx-2];
      sem_wait(&dma_lock);
      write_data_to_ACC(DETECTOR0_START_ADDR, start_of_section, DOUBLE_CHANNEL_WIDTH + 2);
      sem_post(&dma_lock);
      
      sem_wait(&dma_lock);
      read_data_from_ACC(DETECTOR0_READ_ADDR, return_buffer.uc, 4);
      sem_post(&dma_lock);
      unsigned spike_train = (return_buffer).sint;
      for (int i = 0; i <DOUBLE_CHANNEL_WIDTH; i ++){
        //printf("%d: %d\n", idx+i, (spike_train & (1 << i)) >> i );
        output_signal[idx-1+i] = ((spike_train & (1 << i)) >> i);
      }
      // putc('\n', stdout);
      idx += DOUBLE_CHANNEL_WIDTH;
    }
  }
  else{
    while(idx<hart_start+num_of_term){
      start_of_section = &input_signal[idx-2];
      sem_wait(&dma_lock);
      write_data_to_ACC(DETECTOR1_START_ADDR, start_of_section, DOUBLE_CHANNEL_WIDTH + 2);
      sem_post(&dma_lock);

      sem_wait(&dma_lock);
      read_data_from_ACC(DETECTOR1_READ_ADDR, return_buffer.uc, 4);
      sem_post(&dma_lock);
      unsigned spike_train = (return_buffer).sint;
      for (int i = 0; i <DOUBLE_CHANNEL_WIDTH; i ++){
        //printf("%d: %d\n", idx+i, (spike_train & (1 << i)) >> i );
        output_signal[idx-1+i] = ((spike_train & (1 << i)) >> i);
      }
      // putc('\n', stdout);
      idx += DOUBLE_CHANNEL_WIDTH;
    }
  }
    
  if(hart_id == 1)
    done_cal_1 = true;
  while(done_cal_1==false);

  if (hart_id == 0){
  printf("======================================\n");
  printf("\t  Writing to Output \n");
  printf("======================================\n");

  open_output_file("output.bin");

  write_value_to_output(0);
  for(int i = 0; i< SIGNAL_LENGTH-1; i++)
    write_value_to_output(output_signal[i]);
  
  close_output_file();
  }
  
}
