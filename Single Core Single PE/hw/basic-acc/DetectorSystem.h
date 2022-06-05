#ifndef DETECTOR_SYSTEM_H_
#define DETECTOR_SYSTEM_H_
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "define.h"

#include "NEO.h"
#include "ThresholdingModule.h"


const int DETECTOR_READ_ADDR = 0x00000000;
const int DETECTOR_RESULT_ADDR = 0x01000000;

using namespace sc_core;
class DetectorSystem: public sc_module
{
	public:
		NEO neo_1;
		NEO neo_2;
		ThresholdingModule thresholding_module;
		sc_clock clk;
		sc_signal<bool> rst;

	  	tlm_utils::simple_target_socket<DetectorSystem> tsock;
		SC_HAS_PROCESS( DetectorSystem );
		DetectorSystem(sc_module_name n);
		~DetectorSystem() {}
	private:




	sc_fifo<input_channel_type> value_1;
	sc_fifo<input_channel_type> value_2;
	sc_fifo<output_channel_type> result1;
	sc_fifo<output_channel_type> result2;
	sc_fifo<return_channel_type> spike_result;



	sc_fifo_out<input_channel_type> o_value_1;
	sc_fifo_out<input_channel_type> o_value_2;
	sc_fifo_in<return_channel_type> i_spike_result;


	input_channel_type input_values;
	return_channel_type spike_results;
	input_type input_buffer[BUFFER_SIZE];
	void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay);

};



#endif
