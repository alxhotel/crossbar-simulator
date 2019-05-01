#ifndef SINGLEGATE_H
#define SINGLEGATE_H

#include "Operation.h"

class SingleGate : public Operation {
public:
	// Direction
	static const int DIR_LEFT = -1;
	static const int DIR_RIGHT = 1;

	SingleGate(std::string gate, int direction, int qubit_index, int line_number = 0);
	
	void check_static_constraints(CrossbarModel* model);
	void check_left_side(CrossbarModel* model, int origin_i, int origin_j, int left_j, int right_j);
	void check_right_side(CrossbarModel* model, int origin_i, int origin_j, int left_j, int right_j);
	
	void add_dynamic_constraints(CrossbarModel* model, int curr_cycle);
	
	void execute(CrossbarModel* model, int curr_cycle, bool with_animation = false, int speed = 1);
	
	std::vector<int> get_involved_qubits() {
		return {qubit_index};
	}
	
	friend std::ostream& operator<<(std::ostream &strm, const SingleGate &gate) {
		return strm << "SingleGate " << std::to_string(gate.qubit_index)
				<< " dir " << std::to_string(gate.direction);
	}
	
private:
	std::string gate;
	int qubit_index;
	int direction;
	
	int get_wave_hash();
	int get_string_hash(const std::string& str);
};

#endif /* SINGLEGATE_H */
