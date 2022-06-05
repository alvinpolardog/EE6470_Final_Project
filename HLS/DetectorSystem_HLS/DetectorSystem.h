#ifndef DETECTOR_SYSTEM_H_
#define DETECTOR_SYSTEM_H_
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "define.h"
#include "Testbench.h"

#ifndef NATIVE_SYSTEMC
#include "NEO_wrap.h"
#include "ThresholdingModule_wrap.h"
#else
#include "NEO.h"
#include "ThresholdingModule.h"
#endif

const int DETECTOR_READ_ADDR = 0x00000000;
const int DETECTOR_RESULT_ADDR = 0x01000000;

using namespace sc_core;
class DetectorSystem: public sc_module
{
	public:
		SC_HAS_PROCESS( DetectorSystem );
		DetectorSystem(sc_module_name n, std::string input_file, std::string output_file);
		~DetectorSystem();
	private:
		Testbench tb;

	#ifndef NATIVE_SYSTEMC
		NEO_wrapper neo_1;
		NEO_wrapper neo_2;
		ThresholdingModule_wrapper thresholding_module;
	#else
		NEO neo_1;
		NEO neo_2;
		ThresholdingModule thresholding_module;
	#endif

	sc_clock clk;
	sc_signal<bool> rst;


	#ifndef NATIVE_SYSTEMC
		cynw_p2p<input_channel_type> value_1;
		cynw_p2p<input_channel_type> value_2;
		cynw_p2p<output_channel_type> result1;
		cynw_p2p<output_channel_type> result2;
		cynw_p2p<return_channel_type> spike_result;
	#else
		sc_fifo<input_channel_type> value_1;
		sc_fifo<input_channel_type> value_2;
		sc_fifo<output_channel_type> result1;
		sc_fifo<output_channel_type> result2;
		sc_fifo<return_channel_type> spike_result;
	#endif

	std::string _output_file;
	// void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay)

};



#endif
