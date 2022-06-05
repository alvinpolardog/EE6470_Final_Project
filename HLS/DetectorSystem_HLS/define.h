#ifndef _DEFINE_H_
#define _DEFINE_H_

#define INPUT_WIDTH 8
#define OUTPUT_WIDTH 16
#define CHANNEL_WIDTH 2
#define SIGNAL_LENGTH 2000

typedef sc_dt::sc_int<INPUT_WIDTH> input_type;
typedef sc_dt::sc_bigint<INPUT_WIDTH*(CHANNEL_WIDTH+2)> input_channel_type;
typedef sc_dt::sc_int<OUTPUT_WIDTH> output_type;
typedef sc_dt::sc_bigint<OUTPUT_WIDTH*CHANNEL_WIDTH> output_channel_type;
typedef sc_dt::sc_int<CHANNEL_WIDTH*2> return_channel_type;


#endif
