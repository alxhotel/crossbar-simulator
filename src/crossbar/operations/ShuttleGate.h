#ifndef SHUTTLEGATE_H
#define SHUTTLEGATE_H

#include <tuple>
#include <regex>
#include <string>
#include <iostream>
#include "Operation.h"

class ShuttleGate : public Operation {
public:
	// Direction
	static const int DIR_LEFT = -1;
	static const int DIR_RIGHT = 1;
	
	ShuttleGate(int direction, int qubit_index, int line_number = 0);
	
	void check_static_constraints(CrossbarModel* model);
	void check_left_side(CrossbarModel* model, int origin_i, int origin_j, int left_j, int right_j);
	void check_right_side(CrossbarModel* model, int origin_i, int origin_j, int left_j, int right_j);
	
	void add_dynamic_constraints(CrossbarModel* model, int curr_cycle);
	
	void execute(CrossbarModel* model, int curr_cycle, bool with_animation = false, int speed = 1);
	
	std::vector<int> get_involved_qubits() {
		return {qubit_index};
	}
	
	friend std::ostream& operator<<(std::ostream &strm, const ShuttleGate &gate) {
		return strm << "ShuttleGate " << std::to_string(gate.qubit_index)
				<< " dir " << std::to_string(gate.direction);
	}
	
private:
	int qubit_index;
	int direction;
};

#endif /* SHUTTLEGATE_H */
