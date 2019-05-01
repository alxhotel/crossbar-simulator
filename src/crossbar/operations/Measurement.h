#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <cmath>
#include "Operation.h"

class Measurement : public Operation {
public:
	// Ancilla direction
	static const int DIR_ANCILLA_LEFT = 1;
	static const int DIR_ANCILLA_RIGHT = 2;

	// Site direction
	static const int DIR_SITE_UP = 3;
	static const int DIR_SITE_DOWN = 4;
	
	Measurement(int ancilla_direction, int site_direction, int qubit_index, int line_number = 0);
	
	void check_static_constraints(CrossbarModel* model);
	
	void add_dynamic_constraints(CrossbarModel* model, int curr_cycle);
	
	void execute(CrossbarModel* model, int curr_cycle, bool with_animation = false, int speed = 1);
	
	std::vector<int> get_involved_qubits() {
		return {qubit_index};
	}
	
	friend std::ostream& operator<<(std::ostream &strm, const Measurement &gate) {
		return strm << "Measurement " << std::to_string(gate.qubit_index)
				<< ", " << std::to_string(gate.ancilla_direction)
				<< " dir " << std::to_string(gate.site_direction);
	}
	
private:
	int qubit_index;
	int ancilla_direction;
	int site_direction;
};

#endif /* MEASUREMENT_H */
