#include "ConstraintChecker.h"

/**
 * Get intervals by parallel operations.
 * @param operations
 * @return intervals of operations
 */
Intervals::IntervalTree<int, Operation*> ConstraintChecker::get_intervals_by_operations(std::vector<std::vector<Operation*> > operations) {
	Intervals::IntervalTree<int, Operation*> operations_interval;
	
	// TODO: get value from JSON file
	int CYCLE_TIME = 10;
	
	int curr_cycle = 0;
	for (std::vector<Operation*> p_operations : operations) {
		// Check if wait operation
		if (p_operations.size() == 1 && typeid(*p_operations.front()) == typeid(Wait)) {
			Wait* wait_op = dynamic_cast<Wait*>(p_operations.front());
			curr_cycle += wait_op->get_cycle_duration(CYCLE_TIME);
			continue;
		}
		
		// Insert in intervals
		for (Operation* operation : p_operations) {
			operations_interval.insert({curr_cycle, curr_cycle + operation->get_cycle_duration(CYCLE_TIME), operation});
		}
		
		curr_cycle++;
	}
	
	return operations_interval;
}

/**
 * Get the max cycle of all the intervals
 * @param intervals
 * @return 
 */
int ConstraintChecker::max_cycle(Intervals::IntervalTree<int, Operation*> interval_tree) {
	int max_cycle = 0;
	for (const auto &interval : interval_tree.intervals()) {
		if (interval.high > max_cycle) {
			max_cycle = interval.high;
		}
	}
	
	return max_cycle + 1;
}

/**
 * Validates the list of parallel operations
 * @param model
 * @param operations
 * @return the line number with the constraint error, if any
 */
int ConstraintChecker::validate(CrossbarModel* model, std::vector<std::vector<Operation*> > operations) {
	// Get params
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	// First iteration to collect info about instructions and times
	Intervals::IntervalTree<int, Operation*> operations_interval = get_intervals_by_operations(operations);
	
	int max_cycle = ConstraintChecker::max_cycle(operations_interval);
	for (int curr_cycle = 0; curr_cycle < max_cycle; curr_cycle++) {
		// Get intersected intervals
		const auto &intervals = operations_interval.findIntervalsContainPoint(curr_cycle);
		
		// Try to execute always
		for (const auto &interval : intervals) {
			if (interval.low <= curr_cycle && interval.high >= curr_cycle) {
				Operation* operation = interval.value;
				operation->execute(model, curr_cycle - interval.low);
			}
		}
		
		// Operation is executing
		std::vector<Intervals::Interval<int, Operation*> > current_intervals;
		for (const auto &interval : intervals) {
			if (interval.high != curr_cycle) {
				current_intervals.push_back(interval);
			}
		}

		ConstraintChecker::solve_parameters(model, current_intervals, curr_cycle);
		
		// Apply the solution
		for (int k = 0; k < m - 1; k++) {
			if (model->get_h_line_constraint(k)->value() == 0) {
				model->raise_h_line(k);
			} else {
				model->lower_h_line(k);
			}

			if (model->get_v_line_constraint(k)->value() == 0) {
				model->raise_v_line(k);
			} else {
				model->lower_v_line(k);
			}
		}
		for (int k = -1 * (m - 1); k <= (m - 1); k++) {
			if (model->get_d_line(k) != model->get_d_line_constraint(k)->value()) {
				model->set_d_line(k, model->get_d_line_constraint(k)->value());
			}
		}
		/*if (model->get_active_wave() != 0 && this->model->get_wave_constraint()->value() == 0) {
			model->toggle_wave(this->model->get_wave_column_constraint()->value());
		}*/
		
		// Operation is starting
		for (const auto &interval : intervals) {
			if (interval.low == curr_cycle) {
				Operation* operation = interval.value;
				try {
					operation->check_static_constraints(model);
				} catch (std::runtime_error e) {
					throw std::runtime_error(std::string(e.what())
							+ " at line " + std::to_string(operation->get_line_number()));
				}
			}
		}
	}
	
	return 0;
}

/**
 * Check the dynamic constraints and solve the problem
 * @param model
 * @param operations
 */
void ConstraintChecker::solve_parameters(CrossbarModel* model, std::vector<Intervals::Interval<int, Operation*> > intervals, int curr_cycle) {
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	// 1. Add dynamic constraints
	model->add_constraints();
	for (const auto &interval : intervals) {
		Operation* operation = interval.value;
		operation->add_dynamic_constraints(model, curr_cycle - interval.low);
	}
	
	// Get the barriers that are going to be lowered
	std::vector<int> h_line;
	std::vector<int> v_line;
	minimize_problem(model);
	for (int k = 0; k < m - 1; k++) {
		h_line.push_back(model->get_h_line_constraint(k)->value());
		v_line.push_back(model->get_v_line_constraint(k)->value());
	}
	
	// RE-add dynamic constraints
	model->add_constraints();
	std::vector<int> involved_qubits;
	for (const auto &interval : intervals) {
		Operation* operation = interval.value;
		operation->add_dynamic_constraints(model, curr_cycle - interval.low);

		std::vector<int> qubits_involved = operation->get_involved_qubits();
		involved_qubits.insert(involved_qubits.end(), qubits_involved.begin(), qubits_involved.end()); 
	}
	
	naxos::NsProblemManager* pm = model->get_problem_manager();
	
	// 2. Get all qubits not involved in the dynamic constraints
	for (int k = 0; k < m - 1; k++) {
		if (h_line[k] == 1) {
			for (int j = 0; j < n; j++) {
				// Get qubits
				std::set<int> bottom_qubits = model->get_qubits(k, j);
				std::set<int> top_qubits = model->get_qubits(k + 1, j);

				if (!contains(bottom_qubits, involved_qubits)
					&& !contains(top_qubits, involved_qubits)) {
					// Check for adjacent qubits
					if (bottom_qubits.size() > 0 && top_qubits.size() > 0) {
						throw std::runtime_error("Two qubits vertically adjacent in line "
								+ std::to_string(k));
					}
					// Check alone qubits
					if ((bottom_qubits.size() + top_qubits.size()) == 1) {
						naxos::NsIntVar* left_line = model->get_d_line_constraint(j - (k + 1));
						naxos::NsIntVar* right_line = model->get_d_line_constraint(j - k);

						if (bottom_qubits.size() > 0) {
							pm->add(*left_line < *right_line);
						} else {
							pm->add(*left_line > *right_line);	
						}
					}
				}
			}
		}

		if (v_line[k] == 1) {
			for (int i = 0; i < m; i++) {
				// Get qubits
				std::set<int> left_qubits = model->get_qubits(i, k);
				std::set<int> right_qubits = model->get_qubits(i, k + 1);

				if (!contains(left_qubits, involved_qubits)
					&& !contains(right_qubits, involved_qubits)) {
					// Check for adjacent qubits
					if (left_qubits.size() > 0 && right_qubits.size() > 0) {
						throw std::runtime_error("Two qubits horizontally adjacent in line "
								+ std::to_string(k));
					}
					// Check alone qubits
					if ((left_qubits.size() + right_qubits.size()) == 1) {
						naxos::NsIntVar* left_line = model->get_d_line_constraint(k - i);
						naxos::NsIntVar* right_line = model->get_d_line_constraint((k + 1) - i);

						if (left_qubits.size() > 0) {
							pm->add(*left_line > *right_line);
						} else {
							pm->add(*left_line < *right_line);	
						}
					}
				}
			}
		}
	}
	
	// 3. Get the best solution (preferable closed barriers and low voltage)
	minimize_problem(model);
}

void ConstraintChecker::minimize_problem(CrossbarModel* model) {
	// Get params
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	naxos::NsProblemManager* pm = model->get_problem_manager();
	
	// Add objective to minimize
	naxos::NsIntVarArray vObjectiveTerms;
	for (int k = 0; k < m - 1; k++) {
		vObjectiveTerms.push_back(*model->get_h_line_constraint(k));
		vObjectiveTerms.push_back(*model->get_v_line_constraint(k));
	}
	for (int k = -1 * (n - 1); k <= (m - 1); k++) {
		vObjectiveTerms.push_back(*model->get_d_line_constraint(k));
	}
	vObjectiveTerms.push_back(*model->get_wave_constraint());
	vObjectiveTerms.push_back(*model->get_wave_column_constraint());
	
	// Add labeling
	pm->addGoal(new naxos::NsgLabeling(vObjectiveTerms));
	
	try {
		pm->minimize(naxos::NsSum(vObjectiveTerms));
	} catch (...) {
		// Ignore
		std::cout << "Ignore error minimize" << std::endl << std::flush;
	}
	
	if (!pm->nextSolution()) {
		throw std::runtime_error("Conflict between parallel operations");
	} else {
		// Minimize
		/*do {
			// Save solution
			model->save_constraint_solution();
		} while (pm->nextSolution());*/
		
		// Restore solution
		//model->restore_constraint_solution();
	}
}

/**
 * Check if a vector contains a set
 */
bool ConstraintChecker::contains(std::set<int> qubits, std::vector<int> involved_qubits) {
	if (qubits.empty()) return false;
	
	for (int qubit_index : qubits) {
		if (std::find(involved_qubits.begin(), involved_qubits.end(), qubit_index) != involved_qubits.end()) {
			return true;
		}
	}
	
	return false;
}
