#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <iostream>
#include <string>
using namespace std;

#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "define.h"



class Testbench : public sc_module {
public:
	sc_in_clk i_clk;
	sc_out < bool >  o_rst;
  	string output_file_name;

	#ifndef NATIVE_SYSTEMC
		cynw_p2p<input_channel_type>::out o_value_1;
		cynw_p2p<input_channel_type>::out o_value_2;
		cynw_p2p<return_channel_type>::in i_spike_result;
	#else
		sc_fifo_out<input_channel_type> o_value_1;
		sc_fifo_out<input_channel_type> o_value_2;
		sc_fifo_in<return_channel_type> i_spike_result;

	#endif

	input_channel_type input_values;
	return_channel_type spike_results;
	input_type value;

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n, string output_file);
  ~Testbench();

  void open_input_file(string infile_name);


private:
  void pass_input_values();

  input_type read_value_from_input(bool& eof);
  void close_input_file();

  void open_output_file(string output_file_name);
  void write_value_to_output(output_type value);
  void close_output_file();


  ifstream stim_file;      
  ofstream resp_file;
  
	unsigned int n_txn;
	sc_time max_txn_time;
	sc_time min_txn_time;
	sc_time total_txn_time;
	sc_time total_start_time;
	sc_time total_run_time;

	void fetch_result();
  int clock_cycle(sc_time time);

};
#endif
