#ifndef WAIT_H
#define WAIT_H

#include "Operation.h"

class Wait : public Operation {
public:
	Wait(int cycles, int line_number = 0);
	
	void check_static_constraints(CrossbarModel* model);
	
	void add_dynamic_constraints(CrossbarModel* model, int curr_cycle);
	
	void execute(CrossbarModel* model, int curr_cycle, bool with_animation = false, int speed = 1);
	
	std::vector<int> get_involved_qubits() {
		return {};
	}
	
	int get_cycle_duration(int cycle_time) {
		return this->cycles;
	}
	
	friend std::ostream& operator<<(std::ostream &strm, const Wait &gate) {
		return strm << "Wait " << std::to_string(gate.cycles);
	}
	
private:
	int cycles;
};

#endif /* WAIT_H */
