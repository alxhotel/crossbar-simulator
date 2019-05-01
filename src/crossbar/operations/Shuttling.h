#ifndef SHUTTLING_H
#define SHUTTLING_H

#include <regex>
#include <string>
#include "Operation.h"

class Shuttling : public Operation {
public:
	// Directions
	static const int DIR_UP = 1;
	static const int DIR_DOWN = 2;
	static const int DIR_LEFT = 3;
	static const int DIR_RIGHT = 4;
	
	Shuttling(int direction, int qubit_index, int line_number = 0);
	
	void check_static_constraints(CrossbarModel* model);
	
	void add_dynamic_constraints(CrossbarModel* model, int curr_cycle);
	
	void execute(CrossbarModel* model, int curr_cycle, bool with_animation = false, int speed = 1);
	
	std::vector<int> get_involved_qubits() {
		return {qubit_index};
	}
	
	friend std::ostream& operator<<(std::ostream &strm, const Shuttling &gate) {
		return strm << "Shuttling " << std::to_string(gate.qubit_index)
				<< " dir " << std::to_string(gate.direction);
	}
	
private:
	int qubit_index;
	int direction;
};

#endif /* SHUTTLING_H */
