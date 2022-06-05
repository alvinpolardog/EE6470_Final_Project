#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

#include <esc.h>                // for the latency logging functions


#include "Testbench.h"

#include <queue>
static std::queue<sc_time> time_queue;


// Testbench Constructor
Testbench::Testbench(sc_module_name n, string output_file) : sc_module(n), output_file_name(output_file){
  SC_THREAD(pass_input_values);
  sensitive << i_clk.pos();
  dont_initialize();
  SC_THREAD(fetch_result);
  sensitive << i_clk.pos();
  dont_initialize();

  #ifndef NATIVE_SYSTEMC
	o_value_1.clk_rst(i_clk, o_rst);
  o_value_2.clk_rst(i_clk, o_rst);
	i_spike_result.clk_rst(i_clk, o_rst);
	#endif
}

// Testbench Destructor
Testbench::~Testbench() {
	cout << "Total run time = " << total_run_time << endl;
}


// Open the input file, and start input stream
void Testbench::open_input_file(string input_file_name){
  const char * infile_name = input_file_name.c_str();
  stim_file.open(infile_name);
  if(stim_file.fail()){
    cerr << "Couldn't open "<< infile_name << "for reading." <<endl;
    exit(0);
  }
}

// Read value from the input file through the input stream 'stim_file'
input_type Testbench::read_value_from_input(bool & eof){
  input_type value;
  stim_file >> std::ws;
  eof = ( stim_file.eof() );
  if(!stim_file.eof()){
      stim_file >> value;
      eof = ( stim_file.eof() );
  }
  return value;
}

// Close input stream
void Testbench::close_input_file(){
  stim_file.close();
}

// Read over input file, 
// bundling values according to CHANNEL_WIDTH,
// and send it to the NEO module
void Testbench::pass_input_values() {
	n_txn = 0;
	max_txn_time = SC_ZERO_TIME;
	min_txn_time = SC_ZERO_TIME;
	total_txn_time = SC_ZERO_TIME;

#ifndef NATIVE_SYSTEMC
	o_value_1.reset();
  o_value_2.reset();
#endif
	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);
	total_start_time = sc_time_stamp();

  bool eof = false;

  // Prepare Buffer
  for (int i = 1; i <= 2;i++){
      if(eof)
        value = 0;
      else
        value = read_value_from_input(eof);
      // cout<<value<<endl;
      input_values.range(i*INPUT_WIDTH-1,(i-1)*INPUT_WIDTH) = value;
    }

  // Start reading rest of data
  for(int idx = 2; idx < SIGNAL_LENGTH;){
    int i = 1;
    for (i = 3; i <= CHANNEL_WIDTH +2;i++){
      if(eof)
        value = 0;
      else
        value = read_value_from_input(eof);
      // cout<<value<<endl;
      input_values.range(i*INPUT_WIDTH-1,(i-1)*INPUT_WIDTH) = value;
      idx++;
    }

    time_queue.push( sc_time_stamp() );
    #ifndef NATIVE_SYSTEMC
          o_value_1.put(input_values);
    #else
          o_value_1.write(input_values);
    #endif

    input_values.range(2*INPUT_WIDTH-1,0) = input_values.range((i-1)*INPUT_WIDTH-1,(i-3)*INPUT_WIDTH);

    for (i = 3; i <= CHANNEL_WIDTH+2;i++){
      if(eof)
        value = 0;
      else
        value = read_value_from_input(eof);
      // cout<<value<<endl;
      input_values.range(i*INPUT_WIDTH-1,(i-1)*INPUT_WIDTH) = value;
      idx++;
    }

    #ifndef NATIVE_SYSTEMC
          o_value_2.put(input_values);
    #else
          o_value_2.write(input_values);
    #endif

    input_values.range(2*INPUT_WIDTH-1,0) = input_values.range((i-1)*INPUT_WIDTH-1,(i-3)*INPUT_WIDTH);
  }

  wait(10000);
  close_input_file();
  cerr << name() << " Error! Input timed out!" << endl;

#ifndef NATIVE_SYSTEMC
  esc_stop();
#else
  sc_stop();
#endif
}


// Open output file stream
void Testbench::open_output_file( string output_file_name ){
  // put output file in appropriate directory in bdw_work
  std::string filename = getenv( "BDW_SIM_CONFIG_DIR" );
  filename += "/";
  filename += output_file_name;
  cout<<filename<<endl;
  resp_file.open( filename.c_str() );
  if( resp_file.fail()){
      cerr << "Couldn't open " << filename << " for writing." << endl;
      exit( 0 );
  }
}

// Write value to output stream
void Testbench::write_value_to_output(output_type value ){
    resp_file << value << endl;
}

// Close output stream
void Testbench::close_output_file(){
  resp_file.close();
}


// Receive bundled results
// decompose the results and write to output stream
void Testbench::fetch_result() {
#ifndef NATIVE_SYSTEMC
	i_spike_result.reset();
#endif
  open_output_file(output_file_name);

	wait(5);
	wait(1);
  unsigned long total_latency = 0;
  int neo_value = 0;

  int i = 0;
  while(i < SIGNAL_LENGTH-2){
#ifndef NATIVE_SYSTEMC
			return_channel_type output_array = i_spike_result.get();
#else
			return_channel_type output_array = i_spike_result.read();
#endif

      sc_time sent_time( time_queue.front() );
      time_queue.pop();
      unsigned long latency = clock_cycle( sc_time_stamp() - sent_time );
      total_latency += latency;
      cout << "Latency for sample " << i << " is " <<  latency << endl;

      int array_idx = 0;
      while(i<SIGNAL_LENGTH-2){
        output_type neo_value = 0;
        neo_value.range(0,0) = output_array.range(array_idx,array_idx);
        // cout<<"Spike Result #"<< i << " : "<<neo_value<<endl;
        write_value_to_output(neo_value);
        i++;
        if ((++array_idx) == CHANNEL_WIDTH*2)
          break;
      }
  }
  unsigned long average_latency = (total_latency / (SIGNAL_LENGTH-2)) + 1;
  cout << "Testbench sink thread read " << SIGNAL_LENGTH << " values. Average latency " << average_latency <<  "." << endl;

	total_run_time = sc_time_stamp() - total_start_time;


  close_output_file();
#ifndef NATIVE_SYSTEMC
  esc_stop();
#else
  sc_stop();
#endif
}


// Solve for clock cycle from sc_time
int Testbench::clock_cycle( sc_time time )
{
    sc_clock * clk_p = DCAST < sc_clock * >( i_clk.get_interface() );
    sc_time clock_period = clk_p->period(); // get period from the sc_clock object.
    return ( int )( time / clock_period );

}


