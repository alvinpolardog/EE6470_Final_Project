#include <cmath>
#include <cstdlib>


#include "ThresholdingModule.h"

// NEO Module Constructor:
// create thread and set up interfaces
ThresholdingModule::ThresholdingModule( sc_module_name n ): sc_module( n )
{

	SC_THREAD( detect_spike );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
}

ThresholdingModule::~ThresholdingModule() {}

// Main thread of NEO Module
// Read from interface, solve for NEO, and return results
void ThresholdingModule::detect_spike() {
	{
		//wait();
	}
	// Main Loop
	while (true) {

			buffer_sum = 0;
			output_array = 0;

			input_array_1 = i_result_1.read();

			for(int i = 1; i <= CHANNEL_WIDTH; i ++){
				int start = i*INPUT_WIDTH-1;
				int end = (i-1)*INPUT_WIDTH;
				buffer_sum = buffer_sum + input_array_1.range(start,end);
			}


			input_array_2 = i_result_2.read();

			for(int i = 1; i <= CHANNEL_WIDTH; i ++){
				int start = i*INPUT_WIDTH-1;
				int end = (i-1)*INPUT_WIDTH;
				buffer_sum = buffer_sum + input_array_2.range(start,end);
			}

			threshold = 10*(buffer_sum / (CHANNEL_WIDTH*2));

			for(int i = 1; i <= CHANNEL_WIDTH; i ++){
				if (input_array_1.range(i*OUTPUT_WIDTH-1,(i-1)*OUTPUT_WIDTH) > threshold)
					output_array.range(i-1,i-1) = 1;
				else
					output_array.range(i-1,i-1) = 0;
			}

			for(int i = 1; i <= CHANNEL_WIDTH; i ++){
				if (input_array_2.range(i*OUTPUT_WIDTH-1,(i-1)*OUTPUT_WIDTH) > threshold)
					output_array.range(i-1+CHANNEL_WIDTH,i-1+CHANNEL_WIDTH) = 1;
				else
					output_array.range(i-1+CHANNEL_WIDTH,i-1+CHANNEL_WIDTH) = 0;
			}

			// Return results

				o_spike_result.write(output_array);
	}
}
