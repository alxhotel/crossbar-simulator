#ifndef MATCHER_H
#define MATCHER_H

#include "operations/Operation.h"
#include "operations/Shuttling.h"
#include "operations/ZGate.h"

class Matcher {
public:
	static Operation* match(std::string line);
};

#endif /* MATCHER_H */

