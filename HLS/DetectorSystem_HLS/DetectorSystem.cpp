#include "DetectorSystem.h"
DetectorSystem::DetectorSystem(sc_module_name n, std::string input_file, std::string output_file): sc_module( n ), tb("tb", output_file), neo_1("neo_1"), neo_2("neo_2"), thresholding_module("thresholding_module"),
	clk("clk", CLOCK_PERIOD, SC_NS), rst("rst")
{

	neo_1.i_clk(clk);
	neo_1.i_rst(rst);
	neo_2.i_clk(clk);
	neo_2.i_rst(rst);
	thresholding_module.i_clk(clk);
	thresholding_module.i_rst(rst);
	tb.i_clk(clk);
	tb.o_rst(rst);


	tb.o_value_1(value_1);
	tb.o_value_2(value_2);
	neo_1.i_value(value_1);
	neo_2.i_value(value_2);

	neo_1.o_result(result1);
	neo_2.o_result(result2);
	thresholding_module.i_result_1(result1);
	thresholding_module.i_result_2(result2);

	thresholding_module.o_spike_result(spike_result);
	tb.i_spike_result(spike_result);

	tb.open_input_file(input_file);



}

DetectorSystem::~DetectorSystem() {}


