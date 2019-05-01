#ifndef CPHASE_H
#define CPHASE_H

#include <string>
#include <cmath>
#include "Operation.h"

class CPhase : public Operation {
public:
	CPhase(int qubit_index_a, int qubit_index_b, int line_number = 0);
	
	void check_static_constraints(CrossbarModel* model);
	
	void add_dynamic_constraints(CrossbarModel* model, int curr_cycle);
	
	void execute(CrossbarModel* model, int curr_cycle, bool with_animation = false, int speed = 1);
	
	std::vector<int> get_involved_qubits() {
		return {qubit_index_a, qubit_index_b};
	}
	
	friend std::ostream& operator<<(std::ostream &strm, const CPhase &gate) {
		return strm << "CPhase " << std::to_string(gate.qubit_index_a)
				<< ", " << std::to_string(gate.qubit_index_b);
	}
	
private:
	int qubit_index_a;
	int qubit_index_b;
};

#endif /* CPHASE_H */
