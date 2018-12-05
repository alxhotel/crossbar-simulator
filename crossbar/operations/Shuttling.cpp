#include "Shuttling.h"

std::regex Shuttling::PATTERN = std::regex("^shuttle_(up|down|left|left) q\\[(0|[1-9][0-9]*)\\]$");

Shuttling::Shuttling(int direction, int qubit_id) {
	this->direction = direction;
	this->qubit_id = qubit_id;
}

bool Shuttling::matches(std::string line) {
	std::smatch matches;
	return std::regex_match(line, matches, Shuttling::PATTERN);
}

Shuttling* Shuttling::create(std::string line) {
	std::smatch matches;
	if (std::regex_match(line, matches, Shuttling::PATTERN)) {
		if (matches[1] == "up") {
			return new Shuttling(Shuttling::DIR_UP, std::stoi(matches[2]));
		} else if (matches[1] == "down") {
			return new Shuttling(Shuttling::DIR_DOWN, std::stoi(matches[2]));
		} else if (matches[1] == "left") {
			return new Shuttling(Shuttling::DIR_LEFT, std::stoi(matches[2]));
		} else if (matches[1] == "right") {
			return new Shuttling(Shuttling::DIR_RIGHT, std::stoi(matches[2]));
		}
	}
	
	return NULL;
}

bool Shuttling::validate_constraints() {
	// TODO
	return true;
}

void Shuttling::execute(bool with_animation) {
	// TODO
}

