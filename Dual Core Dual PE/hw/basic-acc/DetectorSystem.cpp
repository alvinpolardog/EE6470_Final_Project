#include "DetectorSystem.h"
DetectorSystem::DetectorSystem( sc_module_name n): sc_module( n ), neo_1("neo_1"), neo_2("neo_2"), thresholding_module("thresholding_module"),
	clk("clk"), rst("rst"),tsock("t_skt")
{ //clk("clk", CLOCK_PERIOD, SC_NS)
	tsock.register_b_transport(this, &DetectorSystem::blocking_transport);

	neo_1.i_clk(clk);
	neo_1.i_rst(rst);
	neo_2.i_clk(clk);
	neo_2.i_rst(rst);
	thresholding_module.i_clk(clk);
	thresholding_module.i_rst(rst);

	o_value_1(value_1);
	o_value_2(value_2);
	neo_1.i_value(value_1);
	neo_2.i_value(value_2);

	neo_1.o_result(result1);
	neo_2.o_result(result2);
	thresholding_module.i_result_1(result1);
	thresholding_module.i_result_2(result2);

	thresholding_module.o_spike_result(spike_result);
	i_spike_result(spike_result);


}





void DetectorSystem::blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();
    switch (payload.get_command()) {
		case tlm::TLM_READ_COMMAND:
			{
			sc_dt::sc_uint<16> big_result = 0; 
			return_channel_type results = 0;
			switch (addr) {
				case DETECTOR_RESULT_ADDR:
					results = i_spike_result.read();					
					break;
				//break;
				default:
					std::cerr << "Error! DetectorSystem::blocking_transport in reading" << std::endl;
					break;
			}
			big_result.range(CHANNEL_WIDTH*2-1,0) =results; 
			data_ptr[0] = big_result.range(7,0);
			data_ptr[1] = big_result.range(15,8);
			data_ptr[2] = 0;
			data_ptr[3] = 0;
			break;
			}
		case tlm::TLM_WRITE_COMMAND:
			switch (addr) {
				case DETECTOR_READ_ADDR:
				{
					for (int i = 1; i <= CHANNEL_WIDTH +2;i++)
						input_values.range(i*INPUT_WIDTH-1,(i-1)*INPUT_WIDTH) = data_ptr[i-1];
					o_value_1.write(input_values);
					int j = 1;
					for (int i = CHANNEL_WIDTH+1; i <= CHANNEL_WIDTH*2 +2; i++){
						input_values.range(j*INPUT_WIDTH-1,(j-1)*INPUT_WIDTH) = data_ptr[i-1];
						j++;
					}
					o_value_2.write(input_values);

					break;
				}
				default:
					std::cerr << "Error! DetectorSystem::blocking_transport in writing" << std::endl;
			}
			break;
		case tlm::TLM_IGNORE_COMMAND:
			payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
			return;
		default:
			payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
			return;
    }
    payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}

