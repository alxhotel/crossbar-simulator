#ifndef SHUTTLING_H
#define SHUTTLING_H

#include <regex>
#include <string>
#include "Operation.h"

class Shuttling : public Operation {
public:
	static const int LATENCY_NS = 10;
	
	// Directions
	static const int DIR_UP = 1;
	static const int DIR_DOWN = 2;
	static const int DIR_LEFT = 3;
	static const int DIR_RIGHT = 4;
	
	// K direction
	static const int K_DIR_UP = 1;
	static const int K_DIR_DOWN = -1;
	static const int K_DIR_LEFT = -1;
	static const int K_DIR_RIGHT = 1;
	
	Shuttling(int direction, int qubit_id, int line_number = 0);
	
	void add_static_constraints(CrossbarModel* model);
	
	void add_dynamic_constraints(CrossbarModel* model);
	
	void execute(CrossbarModel* model, bool with_animation = false, int speed = 1);
	
private:
	int qubit_id;
	int direction;
};

#endif /* SHUTTLING_H */

