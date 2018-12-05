#ifndef ZGATE_H
#define ZGATE_H

#include <regex>
#include <string>
#include "Operation.h"

class ZGate : public Operation {
public:
	static std::regex PATTERN;
	
	ZGate(int qubit_id);
	
	static bool matches(std::string line);
	
	static ZGate* create(std::string line);
	
	bool validate_constraints();
	
	void execute(bool with_animation = false);
	
private:
	int qubit_id;
};

#endif /* ZGATE_H */

