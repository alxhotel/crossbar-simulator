#include "ZGate.h"

std::regex ZGate::PATTERN = std::regex("^z q\\[(0|[1-9][0-9]*)\\]$");

ZGate::ZGate(int qubit_id) {
	this->qubit_id = qubit_id;
}

bool ZGate::matches(std::string line) {
	std::smatch matches;
	return std::regex_match(line, matches, ZGate::PATTERN);
}

ZGate* ZGate::create(std::string line) {
	std::smatch matches;
	if (std::regex_match(line, matches, ZGate::PATTERN)) {
		return new ZGate(std::stoi(matches[1]));
	}
	
	return NULL;
}


bool ZGate::validate_constraints() {
	// TODO
	return true;
}
	
void ZGate::execute(bool with_animation) {
	// Try by shuttling
	
	// If not, use global operation
	
}
