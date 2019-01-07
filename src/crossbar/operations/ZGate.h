#ifndef ZGATE_H
#define ZGATE_H

#include <tuple>
#include <regex>
#include <string>
#include <iostream>
#include "Operation.h"

class ZGate : public Operation {
public:
	static const int LATENCY_NS = 100;
	
	// Direction
	static const int K_DIR_LEFT = -1;
	static const int K_DIR_RIGHT = 1;
	
	ZGate(int qubit_id, int line_number = 0);
	
	void add_static_constraints(CrossbarModel* model);
	
	void add_dynamic_constraints(CrossbarModel* model);
	
	void execute(CrossbarModel* model, bool with_animation = false, int speed = 1);
	
private:
	int qubit_id;
};

#endif /* ZGATE_H */

