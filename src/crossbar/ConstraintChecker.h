#ifndef CONSTRAINTCHECKER_H
#define CONSTRAINTCHECKER_H

#include <vector>
#include <naxos.h>
#include "CrossbarModel.h"
#include "operations/Operation.h"
#include "operations/Shuttling.h"
#include "operations/ZGate.h"

class ConstraintChecker {
public:
	static int validate(CrossbarModel* model, std::vector<std::vector<Operation*> > operations);
	
};

#endif /* CONSTRAINTCHECKER_H */

