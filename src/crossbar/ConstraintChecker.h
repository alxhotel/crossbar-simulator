#ifndef CONSTRAINTCHECKER_H
#define CONSTRAINTCHECKER_H

#include <vector>
#include <algorithm>
#include <naxos.h>
#include <interval-tree.h>

#include "CrossbarModel.h"
#include "operations/Operation.h"
#include "operations/Shuttling.h"
#include "operations/SingleGate.h"
#include "operations/Wait.h"

class ConstraintChecker {
public:
	static Intervals::IntervalTree<int, Operation*> get_intervals_by_operations(std::vector<std::vector<Operation*> > operations);
	
	static int max_cycle(Intervals::IntervalTree<int, Operation*> intervals);
	
	static int validate(CrossbarModel* model, std::vector<std::vector<Operation*> > operations);

	static void solve_parameters(CrossbarModel* model,
		std::vector<Intervals::Interval<int, Operation*> > intervals, int curr_cycle);

private:
	static void minimize_problem(CrossbarModel* model);
	
	static bool contains(std::set<int> qubits, std::vector<int> involved_qubits);
};

#endif /* CONSTRAINTCHECKER_H */
