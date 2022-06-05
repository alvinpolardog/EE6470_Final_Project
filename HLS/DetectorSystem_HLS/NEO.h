#ifndef _NEO_H_
#define _NEO_H_

#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "define.h"
#define BUFFER_SIZE 10

class NEO: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in <bool>  i_rst;


#ifndef NATIVE_SYSTEMC
	cynw_p2p<input_channel_type>::in i_value;
	cynw_p2p<output_channel_type>::out o_result;
#else
	sc_fifo_in<input_channel_type> i_value;
	sc_fifo_out<output_channel_type> o_result;
#endif

	SC_HAS_PROCESS( NEO );
	NEO( sc_module_name n );
	~NEO();


private:
	void neo_calculate();
	input_type input_buffer[BUFFER_SIZE];
	input_channel_type input_array;
	output_channel_type output_array;
	sc_dt::sc_int<32> ori_neo_value;
	output_type neo_value;
	input_type value;


	sc_dt::sc_uint<4> current_buffer_idx;
    sc_dt::sc_uint<4> current_neo_idx;
    sc_dt::sc_uint<4> next;
    sc_dt::sc_uint<4> prev;

};

#endif
