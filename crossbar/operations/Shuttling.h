#ifndef SHUTTLING_H
#define SHUTTLING_H

#include <regex>
#include <string>
#include "Operation.h"

class Shuttling : public Operation {
public:
	static std::regex PATTERN;
	
	static const int DIR_UP = 1;
	static const int DIR_DOWN = 2;
	static const int DIR_LEFT = 3;
	static const int DIR_RIGHT = 4;
	
	Shuttling(int direction, int qubit_id);

	static bool matches(std::string line);
	
	static Shuttling* create(std::string line);
	
	bool validate_constraints();
	
	void execute(bool with_animation = false);

private:
	int qubit_id;
	int direction;
};

#endif /* SHUTTLING_H */

