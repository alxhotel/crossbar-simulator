#ifndef SQSWAP_H
#define SQSWAP_H

#include <cmath>
#include "Operation.h"

class SqSwap : public Operation {
public:
	static const int LATENCY_NS = 20;
	
	SqSwap(int qubit_a_id, int qubit_b_id, int line_number = 0);
	
	void add_static_constraints(CrossbarModel* model);
	
	void add_dynamic_constraints(CrossbarModel* model);
	
	void execute(CrossbarModel* model, bool with_animation = false, int speed = 1);
	
private:
	int qubit_a_id;
	int qubit_b_id;
};

#endif /* SQSWAP_H */

