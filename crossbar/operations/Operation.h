#ifndef OPERATION_H
#define OPERATION_H

#include <string>

class Operation {
public:
	static bool matches(std::string line);
	
	static Operation* create(std::string line);
	
	virtual bool validate_constraints() = 0;
	
	virtual void execute(bool with_animations = false) = 0;
};

#endif /* OPERATION_H */

