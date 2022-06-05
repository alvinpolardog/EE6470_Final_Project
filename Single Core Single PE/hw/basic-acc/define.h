#ifndef _DEFINE_H_
#define _DEFINE_H_

#define INPUT_WIDTH 8
#define OUTPUT_WIDTH 16
#define CHANNEL_WIDTH 3
#define SIGNAL_LENGTH 2001

typedef sc_dt::sc_uint<INPUT_WIDTH> input_type;
typedef sc_dt::sc_uint<INPUT_WIDTH*(CHANNEL_WIDTH+2)> input_channel_type;
typedef sc_dt::sc_uint<OUTPUT_WIDTH> output_type;
typedef sc_dt::sc_uint<OUTPUT_WIDTH*CHANNEL_WIDTH> output_channel_type;
typedef sc_dt::sc_uint<CHANNEL_WIDTH*2> return_channel_type;


#endif
