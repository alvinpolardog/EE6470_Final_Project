#include <cmath>
#include <cstdlib>



#include "NEO.h"

// NEO Module Constructor:
// create thread and set up interfaces
NEO::NEO( sc_module_name n ): sc_module( n )
{


	SC_THREAD( neo_calculate );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);


}

NEO::~NEO() {}

// Main thread of NEO Module
// Read from interface, solve for NEO, and return results
void NEO::neo_calculate() {

	{
		//wait();
	}

	// Main Loop
	while (true) {


			input_array = i_value.read();
			// Set up NEO buffer

			current_buffer_idx = 0;
			input_buffer[current_buffer_idx++] = input_array.range(INPUT_WIDTH-1,0);
			input_buffer[current_buffer_idx++] = input_array.range(2*INPUT_WIDTH-1,INPUT_WIDTH);


			
			{
				// Loop over and decompose the input, transfer to internal buffer
				for(int i = 3; i <= CHANNEL_WIDTH+2; i++){

					int start = i*INPUT_WIDTH-1;
					int end = (i-1)*INPUT_WIDTH;
					input_buffer[current_buffer_idx++] = input_array.range(start,end);
				}

			}

			current_neo_idx = 1;
			// Loop over internal buffer, and solve for NEO for each new values, storing it inside the result(output_array)
			for(int i = 1; i <= CHANNEL_WIDTH; i++){		
				next = ((current_neo_idx == (BUFFER_SIZE - 1))?  0 : (current_neo_idx + 1));
				prev = ((current_neo_idx == 0)? BUFFER_SIZE -1 : (current_neo_idx - 1) );
				ori_neo_value = input_buffer[current_neo_idx] * input_buffer[current_neo_idx] - input_buffer[next] * input_buffer[prev];
				if (ori_neo_value<=0)
					ori_neo_value = -ori_neo_value;
				if (ori_neo_value>65500)
					ori_neo_value = 65500;
				neo_value = ori_neo_value;
				//std::cout<<input_buffer[current_neo_idx]<< ' '<<next<<' '<< input_buffer[next]<< ' '<< input_buffer[prev]<< ' ' << neo_value<<std::endl;
				output_array.range(i*OUTPUT_WIDTH-1,(i-1)*OUTPUT_WIDTH) = neo_value;
				// cout<<i<<' '<< "prev "<<prev<<' '<<input_buffer[prev]<<"; cur "<< current_neo_idx<< ' '<< input_buffer[current_neo_idx]<<"; next "<< next<< ' '<< input_buffer[next]<<"; neo_value "<<neo_value<<endl;
				current_neo_idx++;
				if (current_neo_idx == BUFFER_SIZE)
                    current_neo_idx = 0;
			}



			// Return results

			o_result.write(output_array);



	}
}
