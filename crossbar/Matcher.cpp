#include "Matcher.h"

Operation* Matcher::match(std::string line) {
	if (Shuttling::matches(line)) {
		// Shuttling
		return Shuttling::create(line);
		
	} else if (ZGate::matches(line)) {
		// ZGate
		return ZGate::create(line);
	} /*else if (TwoQubitGate::matches(line)) {
		
	} else if (Measurement::matches(line)) {
	
	}*/
	
	return NULL;
}



