#include "string"
#include "string.h"
#include "cassert"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

#define SIGNAL_LENGTH 2000
#define DOUBLE_CHANNEL_WIDTH 6

// Gauss Filter ACC
static char* const DETECTOR_START_ADDR = reinterpret_cast<char* const>(0x73000000);
static char* const DETECTOR_READ_ADDR  = reinterpret_cast<char* const>(0x74000000);

// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = true;
std::ifstream stim_file;
std::ofstream resp_file;

unsigned char  input_signal[SIGNAL_LENGTH+DOUBLE_CHANNEL_WIDTH] = {0};
unsigned char  output_signal[SIGNAL_LENGTH+DOUBLE_CHANNEL_WIDTH] = {0};


void open_input_file(std::string input_file_name){  
  const char * infile_name = input_file_name.c_str();
  stim_file.open(infile_name);
  if(stim_file.fail()){
    std::cerr << "Couldn't open "<< infile_name << "for reading." <<std::endl;
    exit(0);
  }
}

int read_value_from_input(bool & eof){
  int value;
  stim_file >> std::ws;
  eof = ( stim_file.eof() );
  if(!stim_file.eof()){
      stim_file >> value;
      eof = ( stim_file.eof() );
  }
  return value;
}

// Close input stream
void close_input_file(){
  stim_file.close();
}

void open_output_file( std::string output_file_name ){
  // put output file in appropriate directory in bdw_work
  resp_file.open( output_file_name.c_str() );
  if( resp_file.fail()){
      std::cerr << "Couldn't open " << output_file_name << " for writing." << std::endl;
      exit( 0 );
  }
}

void write_value_to_output(unsigned char value ){
  resp_file << int(value) << std::endl;
}

void close_output_file(){
  resp_file.close();
}

void write_data_to_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(buffer);
    *DMA_DST_ADDR = (uint32_t)(ADDR);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;

  }else{
    // Directly Send
    memcpy(ADDR, buffer, sizeof(unsigned char)*len);
  }
}
void read_data_from_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(ADDR);
    *DMA_DST_ADDR = (uint32_t)(buffer);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Read
    memcpy(buffer, ADDR, sizeof(unsigned char)*len);
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

int main(int argc, char *argv[]) {

  printf("======================================\n");
  printf("\t  Reading Stimulus\n");
  printf("======================================\n");
  open_input_file("stimulus.bin");
  bool eof = false;
  int i = 0;
  while(eof == false){
    input_signal[i++] = read_value_from_input(eof);
  }
  close_input_file();




  printf("======================================\n");
  printf("\t  Starting Spike Detection \n");
  printf("======================================\n");

  unsigned char  result_signal[10] = {0};
  unsigned char * start_of_section;
  word return_buffer;
  char char_buffer [8];
 
  int idx = 2;
  int result = 0;

  while(idx<SIGNAL_LENGTH){
    start_of_section = &input_signal[idx-2];
    write_data_to_ACC(DETECTOR_START_ADDR, start_of_section, DOUBLE_CHANNEL_WIDTH + 2);
      //printf("iter\n");

    read_data_from_ACC(DETECTOR_READ_ADDR, return_buffer.uc, 4);
    unsigned spike_train = (return_buffer).sint;
    for (int i = 0; i <DOUBLE_CHANNEL_WIDTH; i ++){
      //printf("%d", (spike_train & (1 << i)) >> i );
       output_signal[idx-1+i] = ((spike_train & (1 << i)) >> i);
    }
    // putc('\n', stdout);
    idx += DOUBLE_CHANNEL_WIDTH;
  }

  printf("======================================\n");
  printf("\t  Writing to Output \n");
  printf("======================================\n");

  open_output_file("output.bin");

  write_value_to_output(0);
  for(int i = 0; i< SIGNAL_LENGTH-1; i++)
    write_value_to_output(output_signal[i]);
  
  close_output_file();

}
