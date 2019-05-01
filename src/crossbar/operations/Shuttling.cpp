#include "Shuttling.h"

Shuttling::Shuttling(int direction, int qubit_index, int line_number) : Operation(0, 20) {
	this->direction = direction;
	this->qubit_index = qubit_index;
	this->line_number = line_number;
}

void Shuttling::check_static_constraints(CrossbarModel* model) {
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	std::set<int> dest_site;
	
	// Constraints
	switch (this->direction) {
		case Shuttling::DIR_UP:
			// Top

			// Handle out of bound in crossbar
			if (origin_i + 1 >= m) {
				throw std::runtime_error("Conflict: Shuttling out of bounds");
			}

			// 1. Empty spot
			dest_site = model->get_qubits(origin_i + 1, origin_j);
			if (!dest_site.empty()) {
				throw std::runtime_error(
					std::string("Conflict: Site destination ")
					+ "(" + std::to_string(origin_i + 1) + ", " + std::to_string(origin_j) + ")"
					+ " is already occupied"
				);
			}
			
			// 2. No two qubits in the same column
			/*for (int j = 0; j < n; j++) {
				if (j == origin_j) continue;
				std::set<int> top_site = model->get_qubits(origin_i + 1, j);
				std::set<int> bottom_site = model->get_qubits(origin_i, j);
				if (!top_site.empty() && !bottom_site.empty()) {
					throw std::runtime_error(
						"Conflict: Two adjacent qubits in the same column " + std::to_string(j)
					);
				}
			}*/

			// 3. Barriers

			// Left barrier
			if (origin_j > 0) {
				if (model->is_v_barrier_down(origin_j - 1)) {
					throw std::runtime_error(
						std::string("Conflict: The vertical barrier ")
						+ std::to_string(origin_j - 1)
						+ " is lowered"
					);
				}
			}

			// Right barrier
			if (origin_j < n - 1) {
				if (model->is_v_barrier_down(origin_j)) {
					throw std::runtime_error(
						std::string("Conflict: The vertical barrier ")
						+ std::to_string(origin_j)
						+ " is lowered"
					);
				}
			}

			// Top barrier
			if (origin_i < m - 2) {
				if (model->is_h_barrier_down(origin_i + 1)) {
					throw std::runtime_error(
						std::string("Conflict: The horizontal barrier ")
						+ std::to_string(origin_i + 1)
						+ " is lowered"
					);
				}
			}

			// Bottom barrier
			if (origin_i > 0) {
				if (model->is_h_barrier_down(origin_i - 1)) {
					throw std::runtime_error(
						std::string("Conflict: The horizontal barrier ")
						+ std::to_string(origin_i - 1)
						+ " is lowered"
					);
				}
			}
			
			break;
		case Shuttling::DIR_DOWN:
			// Down

			// Handle out of bound in crossbar
			if (origin_i - 1 < 0) {
				throw std::runtime_error("Conflict: Shuttling out of bounds");
			}

			// 1. Empty spot
			dest_site = model->get_qubits(origin_i - 1, origin_j);
			if (!dest_site.empty()) {
				throw std::runtime_error(
					std::string("Conflict: Site destination ")
					+ "(" + std::to_string(origin_i - 1) + ", " + std::to_string(origin_j) + ")"
					+ " is already occupied"
				);
			}

			// 2. Two qubits in the same column
			/*for (int j = 0; j < n; j++) {
				if (j == origin_j) continue;
				std::set<int> top_site = model->get_qubits(origin_i, j);
				std::set<int> bottom_site = model->get_qubits(origin_i - 1, j);
				if (!top_site.empty() && !bottom_site.empty()) {
					throw std::runtime_error(
						"Conflict: Two adjacent qubits in the same column " + std::to_string(j)
					);
				}
			}*/

			// 3. Barriers

			// Left barrier
			if (origin_j > 0) {
				if (model->is_v_barrier_down(origin_j - 1)) {
					throw std::runtime_error(
						std::string("Conflict: The vertical barrier ")
						+ std::to_string(origin_j - 1)
						+ " is lowered"
					);
				}
			}

			// Right barrier
			if (origin_j < n - 1) {
				if (model->is_v_barrier_down(origin_j)) {
					throw std::runtime_error(
						std::string("Conflict: The vertical barrier ")
						+ std::to_string(origin_j)
						+ " is lowered"
					);
				}
			}

			// Top barrier
			if (origin_i < m - 1) {
				if (model->is_h_barrier_down(origin_i)) {
					throw std::runtime_error(
						std::string("Conflict: The horizontal barrier ")
						+ std::to_string(origin_i)
						+ " is lowered"
					);
				}
			}

			// Bottom barrier
			if (origin_i > 1) {
				if (model->is_h_barrier_down(origin_i - 2)) {
					throw std::runtime_error(
						std::string("Conflict: The horizontal barrier ")
						+ std::to_string(origin_i - 2)
						+ " is lowered"
					);
				}
			}
			
			break;
		case Shuttling::DIR_LEFT:
			// Left

			// Handle out of bound in crossbar
			if (origin_j - 1 < 0) {
				throw std::runtime_error("Conflict: Shuttling out of bounds");
			}

			// 1. Empty spot
			dest_site = model->get_qubits(origin_i, origin_j - 1);
			if (!dest_site.empty()) {
				throw std::runtime_error(
					std::string("Conflict: Site destination ")
					+ "(" + std::to_string(origin_i) + ", " + std::to_string(origin_j - 1) + ")"
					+ " is already occupied"
				);
			}

			// 2. Two qubits in the same row
			/*for (int i = 0; i < m; i++) {
				if (i == origin_i) continue;
				std::set<int> left_site = model->get_qubits(i, origin_j - 1);
				std::set<int> right_site = model->get_qubits(i, origin_j);
				if (!left_site.empty() && !right_site.empty()) {
					throw std::runtime_error(
						"Conflict: Two adjacent qubits in the same row " + std::to_string(i)
					);
				}
			}*/

			// 3. Barrier

			// Top barrier
			if (origin_i < n - 1) {
				if (model->is_h_barrier_down(origin_i)) {
					throw std::runtime_error(
						std::string("Conflict: The horizontal barrier ")
						+ std::to_string(origin_i)
						+ " is lowered"
					);
				}
			}

			// Bottom barrier
			if (origin_i > 0) {
				if (model->is_h_barrier_down(origin_i - 1)) {
					throw std::runtime_error(
						std::string("Conflict: The horizontal barrier ")
						+ std::to_string(origin_i - 1)
						+ " is lowered"
					);
				}
			}

			// Left barrier
			if (origin_j > 1) {
				if (model->is_v_barrier_down(origin_j - 2)) {
					throw std::runtime_error(
						std::string("Conflict: The vertical barrier ")
						+ std::to_string(origin_j - 2)
						+ " is lowered"
					);
				}
			}

			// Right barrier
			if (origin_j < n - 1) {
				if (model->is_v_barrier_down(origin_j)) {
					throw std::runtime_error(
						std::string("Conflict: The vertical barrier ")
						+ std::to_string(origin_j)
						+ " is lowered"
					);
				}
			}
			
			break;
		case Shuttling::DIR_RIGHT:
			// Right

			// Handle out of bound in crossbar
			if (origin_j + 1 >= n) {
				throw std::runtime_error("Conflict: Shuttling out of bounds");
			}

			// 1. Empty spot
			dest_site = model->get_qubits(origin_i, origin_j + 1);
			if (!dest_site.empty()) {
				throw std::runtime_error(
					std::string("Conflict: Site destination ")
					+ "(" + std::to_string(origin_i) + ", " + std::to_string(origin_j + 1) + ")"
					+ " is already occupied"
				);
			}

			// 2. Two qubits in the same row
			/*for (int i = 0; i < m; i++) {
				if (i == origin_i) continue;
				std::set<int> left_site = model->get_qubits(i, origin_j);
				std::set<int> right_site = model->get_qubits(i, origin_j + 1);
				if (!left_site.empty() && !right_site.empty()) {
					throw std::runtime_error(
						"Conflict: Two adjacent qubits in the same row " + std::to_string(i)
					);
				}
			}*/

			// 3. Barrier

			// Top barrier
			if (origin_i < n - 1) {
				if (model->is_h_barrier_down(origin_i)) {
					throw std::runtime_error(
						std::string("Conflict: The horizontal barrier ")
						+ std::to_string(origin_i)
						+ " is lowered"
					);
				}
			}

			// Bottom barrier
			if (origin_i > 0) {
				if (model->is_h_barrier_down(origin_i - 1)) {
					throw std::runtime_error(
						std::string("Conflict: The horizontal barrier ")
						+ std::to_string(origin_i - 1)
						+ " is lowered"
					);
				}
			}

			// Left barrier
			if (origin_j > 0) {
				if (model->is_v_barrier_down(origin_j - 1)) {
					throw std::runtime_error(
						std::string("Conflict: The vertical barrier ")
						+ std::to_string(origin_j - 1)
						+ " is lowered"
					);
				}
			}

			// Right barrier
			if (origin_j < n - 2) {
				if (model->is_v_barrier_down(origin_j + 1)) {
					throw std::runtime_error(
						std::string("Conflict: The vertical barrier ")
						+ std::to_string(origin_j + 1)
						+ " is lowered"
					);
				}
			}
			
			break;
		default:
			// throw error
			throw std::runtime_error("Invalid shuttling direction");
			break;
	}
}

void Shuttling::add_dynamic_constraints(CrossbarModel* model, int curr_cycle) {
	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	naxos::NsProblemManager* pm = model->get_problem_manager();
    
	switch (this->direction) {
		case Shuttling::DIR_UP: {
			// Sites
			naxos::NsIntVar* origin_site = model->get_position_qubits_constraint(origin_i, origin_j);
			pm->add(*origin_site == this->qubit_index);
			
			naxos::NsIntVar* destination_site = model->get_position_qubits_constraint(origin_i + 1, origin_j);
			pm->add(*destination_site == this->qubit_index);

			// Barriers
			naxos::NsIntVar* middle_barrier = model->get_h_line_constraint(origin_i);
			pm->add(*middle_barrier == 1);

			if (origin_i > 0) {
				naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(origin_i - 1);
				pm->add(*bottom_barrier == 0);
			}
			if (origin_i + 1 < n - 1) {
				naxos::NsIntVar* top_barrier = model->get_h_line_constraint(origin_i + 1);
				pm->add(*top_barrier == 0);
			}
			if (origin_j > 0) {
				naxos::NsIntVar* left_barrier = model->get_v_line_constraint(origin_j - 1);
				pm->add(*left_barrier == 0);
			}
			if (origin_j < n - 1) {
				naxos::NsIntVar* right_barrier = model->get_v_line_constraint(origin_j);
				pm->add(*right_barrier == 0);
			}
			
			// Qubit lines
			naxos::NsIntVar* left_line = model->get_d_line_constraint(origin_j - origin_i - 1);
			naxos::NsIntVar* right_line = model->get_d_line_constraint(origin_j - origin_i);
			pm->add(*left_line > *right_line);
			
			break;
		}
		case Shuttling::DIR_DOWN: {
			// Sites
			naxos::NsIntVar* origin_site = model->get_position_qubits_constraint(origin_i, origin_j);
			pm->add(*origin_site == this->qubit_index);
			
			naxos::NsIntVar* destination_site = model->get_position_qubits_constraint(origin_i - 1, origin_j);
			pm->add(*destination_site == this->qubit_index);
			
			// Barriers
			naxos::NsIntVar* middle_barrier = model->get_h_line_constraint(origin_i - 1);
			pm->add(*middle_barrier == 1);
			
			if (origin_i - 1 > 0) {
				naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(origin_i - 2);
				pm->add(*bottom_barrier == 0);
			}
			if (origin_i < n - 1) {
				naxos::NsIntVar* top_barrier = model->get_h_line_constraint(origin_i);
				pm->add(*top_barrier == 0);
			}
			if (origin_j > 0) {
				naxos::NsIntVar* left_barrier = model->get_v_line_constraint(origin_j - 1);
				pm->add(*left_barrier == 0);
			}
			if (origin_j < n - 1) {
				naxos::NsIntVar* right_barrier = model->get_v_line_constraint(origin_j);
				pm->add(*right_barrier == 0);
			}
			
			// Qubit lines
			naxos::NsIntVar* left_line = model->get_d_line_constraint(origin_j - origin_i);
			naxos::NsIntVar* right_line = model->get_d_line_constraint(origin_j - origin_i + 1);
			pm->add(*left_line < *right_line);
			
			break;
		}
		case Shuttling::DIR_LEFT: {
			// Sites
			naxos::NsIntVar* origin_site = model->get_position_qubits_constraint(origin_i, origin_j);
			pm->add(*origin_site == this->qubit_index);
			
			naxos::NsIntVar* destination_site = model->get_position_qubits_constraint(origin_i, origin_j - 1);
			pm->add(*destination_site == this->qubit_index);
			
			// Barriers
			naxos::NsIntVar* middle_barrier = model->get_v_line_constraint(origin_j - 1);
			pm->add(*middle_barrier == 1);
			
			if (origin_i > 0) {
				naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(origin_i - 1);
				pm->add(*bottom_barrier == 0);
			}
			if (origin_i < n - 1) {
				naxos::NsIntVar* top_barrier = model->get_h_line_constraint(origin_i);
				pm->add(*top_barrier == 0);
			}
			if (origin_j - 1 > 0) {
				naxos::NsIntVar* left_barrier = model->get_v_line_constraint(origin_j - 2);
				pm->add(*left_barrier == 0);
			}
			if (origin_j < n - 1) {
				naxos::NsIntVar* right_barrier = model->get_v_line_constraint(origin_j);
				pm->add(*right_barrier == 0);
			}
			
			// Qubit lines
			naxos::NsIntVar* left_line = model->get_d_line_constraint(origin_j - origin_i - 1);
			naxos::NsIntVar* right_line = model->get_d_line_constraint(origin_j - origin_i);
			pm->add(*left_line > *right_line);
			
			break;
		}
		case Shuttling::DIR_RIGHT: {
			// Sites
			naxos::NsIntVar* origin_site = model->get_position_qubits_constraint(origin_i, origin_j);
			pm->add(*origin_site == this->qubit_index);
			
			naxos::NsIntVar* destination_site = model->get_position_qubits_constraint(origin_i, origin_j + 1);
			pm->add(*destination_site == this->qubit_index);
			
			// Barriers
			naxos::NsIntVar* middle_barrier = model->get_v_line_constraint(origin_j);
			pm->add(*middle_barrier == 1);
			
			if (origin_i > 0) {
				naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(origin_i - 1);
				pm->add(*bottom_barrier == 0);
			}
			if (origin_i < n - 1) {
				naxos::NsIntVar* top_barrier = model->get_h_line_constraint(origin_i);
				pm->add(*top_barrier == 0);
			}
			if (origin_j > 0) {
				naxos::NsIntVar* left_barrier = model->get_v_line_constraint(origin_j - 1);
				pm->add(*left_barrier == 0);
			}
			if (origin_j + 1 < n - 1) {
				naxos::NsIntVar* right_barrier = model->get_v_line_constraint(origin_j + 1);
				pm->add(*right_barrier == 0);
			}
			
			// Qubit lines
			naxos::NsIntVar* left_line = model->get_d_line_constraint(origin_j - origin_i);
			naxos::NsIntVar* right_line = model->get_d_line_constraint(origin_j - origin_i + 1);
			pm->add(*left_line < *right_line);
			
			break;
		}
	}
}

void Shuttling::execute(CrossbarModel* model, int curr_cycle, bool with_animation, int speed) {
	// Evolve at the end by default
	if (this->is_cycle(curr_cycle, 2)) {
		model->evolve(this->get_involved_qubits());
	}
	
	/*Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	int dest_i, dest_j;
	
	switch (this->direction) {
		case Shuttling::DIR_UP:
			dest_i = pos->get_i() + 1;
			dest_j = pos->get_j();
			
			// 1. Horizontal barrier down
			// 2. Apply difference in QL voltages
			if (this->is_cycle(curr_cycle, 0)) {
				model->lower_h_line(origin_i);
				model->apply_diff_ql(origin_i, origin_j, dest_i, dest_j);
				model->evolve();
			}
			
			// 3. Horizontal barrier up
			if (this->is_cycle(curr_cycle, 1)) {
				model->raise_h_line(origin_i);
			}
			break;
		case Shuttling::DIR_DOWN:
			dest_i = pos->get_i() - 1;
			dest_j = pos->get_j();
			
			// 1. Horizontal barrier down
			// 2. Apply difference in QL voltages
			if (this->is_cycle(curr_cycle, 0)) {
				model->lower_h_line(origin_i - 1);
				model->apply_diff_ql(origin_i, origin_j, dest_i, dest_j);
				model->evolve();
			}
			
			// 3. Horizontal barrier up
			if (this->is_cycle(curr_cycle, 1)) {				
				model->raise_h_line(origin_i);
			}
			break;
		case Shuttling::DIR_LEFT:
			dest_i = pos->get_i();
			dest_j = pos->get_j() - 1;
			
			// 1. Vertical barrier down
			// 2. Apply difference in QL voltages
			if (this->is_cycle(curr_cycle, 0)) {
				model->toggle_v_line(origin_j - 1);
				model->apply_diff_ql(origin_i, origin_j, dest_i, dest_j);
				model->evolve();
			}
			
			// 3. Vertical barrier up
			if (this->is_cycle(curr_cycle, 1)) {
				model->toggle_v_line(origin_j - 1);
			}
			break;
		case Shuttling::DIR_RIGHT:
			dest_i = pos->get_i();
			dest_j = pos->get_j() + 1;
			
			// 1. Vertical barrier downn
			// 2. Apply difference in QL voltages
			if (this->is_cycle(curr_cycle, 0)) {
				model->toggle_v_line(origin_j);
				model->apply_diff_ql(origin_i, origin_j, dest_i, dest_j);
				model->evolve();
			}
			
			// 3. Vertical barrier up
			if (this->is_cycle(curr_cycle, 1)) {
				model->toggle_v_line(origin_j);
			}
			break;
		default:
			throw std::runtime_error("Undefined shuttling instruction");
			break;
	}*/
}
