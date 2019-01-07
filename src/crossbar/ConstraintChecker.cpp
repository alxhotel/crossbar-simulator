#include "ConstraintChecker.h"

/**
 * Validates the list of parallel operations
 * @param model
 * @param operations
 * @return the line number with the constraint error, if any
 */
int ConstraintChecker::validate(CrossbarModel* model, std::vector<std::vector<Operation*> > operations) {
	for (std::vector<Operation*> p_operations : operations) {
		// Check static constraints
		for (Operation* operation : p_operations) {
			CrossbarModel* cloned_model = model->clone();
			cloned_model->add_static_constraints();
			operation->add_static_constraints(cloned_model);
			naxos::NsProblemManager* pm = cloned_model->get_problem_manager();
			bool has_solution = pm->nextSolution();
			delete cloned_model;
			if (!has_solution) return operation->getLineNumber();
		}
		
		// Check dynamic constraints
		CrossbarModel* cloned_model = model->clone();
		for (Operation* operation : p_operations) {
			operation->add_dynamic_constraints(model);
		}
		naxos::NsProblemManager* pm = model->get_problem_manager();
		bool has_solution = pm->nextSolution();
		delete cloned_model;
		// TODO: uncomment for parallel operations
		//if (!has_solution) return p_operations[0]->getLineNumber();
		
		// TODO: maybe improve this "simulation"
		// Simulate the execution of parallel operations
		for (Operation* operation : p_operations) {
			operation->execute(model);
		}
	}
	
	return 0;
}