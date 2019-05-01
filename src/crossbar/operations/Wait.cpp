#include "Wait.h"

Wait::Wait(int cycles, int line_number) : Operation(0, 0) {
	this->cycles = cycles;
	this->line_number = line_number;
}

void Wait::check_static_constraints(CrossbarModel* model) {
	
}

void Wait::add_dynamic_constraints(CrossbarModel* model, int curr_cycle) {
	
}

void Wait::execute(CrossbarModel* model, int curr_cycle, bool with_animation, int speed) {
	
}
