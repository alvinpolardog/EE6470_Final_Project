#ifndef _THRESHOLDING_MODULE_H_
#define _THRESHOLDING_MODULE_H_

#include <systemc>
using namespace sc_core;


#include "define.h"
#define BUFFER_SIZE 10

class ThresholdingModule: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in <bool>  i_rst;



	sc_fifo_in<output_channel_type> i_result_1;
	sc_fifo_in<output_channel_type> i_result_2;
	sc_fifo_out<return_channel_type> o_spike_result;


	SC_HAS_PROCESS( ThresholdingModule );
	ThresholdingModule( sc_module_name n );
	~ThresholdingModule();


private:
	void detect_spike();
	input_type input_buffer[BUFFER_SIZE*2];
	output_channel_type input_array_1;
	output_channel_type input_array_2;
	return_channel_type output_array;


	sc_dt::sc_uint<12> buffer_sum;
	input_type threshold;

};

#endif
