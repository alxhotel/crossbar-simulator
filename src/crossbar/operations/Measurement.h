#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <cmath>
#include "Operation.h"

class Measurement : public Operation {
public:
	static const int LATENCY_NS = 100;
	
	Measurement(int qubit_id, int line_number = 0);
	
	void add_static_constraints(CrossbarModel* model);
	
	void add_dynamic_constraints(CrossbarModel* model);
	
	void execute(CrossbarModel* model, bool with_animation = false, int speed = 1);
	
private:
	int qubit_id;
};

#endif /* MEASUREMENT_H */
