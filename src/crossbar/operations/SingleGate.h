#ifndef SINGLEGATE_H
#define SINGLEGATE_H

#include "Operation.h"

class SingleGate : public Operation {
public:
	static const int LATENCY_NS = 1000;
	
	// Direction
	static const int K_DIR_LEFT = -1;
	static const int K_DIR_RIGHT = 1;

	SingleGate(int qubit_id, int line_number = 0);
	
	void add_static_constraints(CrossbarModel* model);
	
	void add_dynamic_constraints(CrossbarModel* model);
	
	void execute(CrossbarModel* model, bool with_animation = false, int speed = 1);
	
private:
	int qubit_id;
};

#endif /* SINGLEGATE_H */

