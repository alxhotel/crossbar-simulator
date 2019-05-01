#include "CPhase.h"

CPhase::CPhase(int qubit_index_a, int qubit_index_b, int line_number) : Operation(0, 80) {
	this->qubit_index_a = qubit_index_a;
	this->qubit_index_b = qubit_index_b;
	this->line_number = line_number;
}

void CPhase::check_static_constraints(CrossbarModel* model) {
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	// Get info
	Qubit* qubit_a = model->get_qubit(this->qubit_index_a);
	Qubit* qubit_b = model->get_qubit(this->qubit_index_b);
	
	QubitPosition* pos_a = qubit_a->get_position();
	QubitPosition* pos_b = qubit_b->get_position();
	
	int origin_a_i = pos_a->get_i();
	int origin_a_j = pos_a->get_j();
	int origin_b_i = pos_b->get_i();
	int origin_b_j = pos_b->get_j();
	
	int origin_left_j, origin_right_j;
	if (origin_b_j < origin_a_j) {
		origin_left_j = origin_a_j;
		origin_right_j = origin_b_j;
	} else {
		origin_left_j = origin_b_j;
		origin_right_j = origin_a_j;
	}
	
	// 1. Both qubits in same column
	if (origin_a_i != origin_b_i) {
		throw std::runtime_error("Conflict: CPhase between qubits in different rows");
	}
	if (abs(origin_a_j - origin_b_j) != 1) {
		throw std::runtime_error("Conflict: CPhase between qubits too far away");
	}
	
	// 2. No two qubits in the same column
	/*for (int j = 0; j < n; j++) {
		if (j == origin_a_j) continue;
		std::set<int> top_site = model->get_qubits(origin_top_i, j);
		std::set<int> bottom_site = model->get_qubits(origin_bottom_i, j);
		if (!top_site.empty() && !bottom_site.empty()) {
			throw std::runtime_error(
				"Conflict: Two adjacent qubits in the same column " + std::to_string(j)
			);
		}
	}*/

	// 3. Barriers
	
	// Top barrier
	if (origin_a_i < m - 1) {
		if (model->is_h_barrier_down(origin_a_i)) {
			throw std::runtime_error(
				std::string("Conflict: The horizontal barrier ")
				+ std::to_string(origin_a_i)
				+ " is lowered"
			);
		}
	}

	// Bottom barrier
	if (origin_a_i > 0) {
		if (model->is_h_barrier_down(origin_a_i - 1)) {
			throw std::runtime_error(
				std::string("Conflict: The horizontal barrier ")
				+ std::to_string(origin_a_i - 1)
				+ " is lowered"
			);
		}
	}
	
	// Left barrier
	if (origin_left_j > 0) {
		if (model->is_v_barrier_down(origin_left_j - 1)) {
			throw std::runtime_error(
				std::string("Conflict: The vertical barrier ")
				+ std::to_string(origin_left_j - 1)
				+ " is lowered"
			);
		}
	}

	// Right barrier
	if (origin_right_j < n - 1) {
		if (model->is_v_barrier_down(origin_right_j)) {
			throw std::runtime_error(
				std::string("Conflict: The vertical barrier ")
				+ std::to_string(origin_right_j)
				+ " is lowered"
			);
		}
	}
}

void CPhase::add_dynamic_constraints(CrossbarModel* model, int curr_cycle) {
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	// Qubit info
	Qubit* qubit_a = model->get_qubit(this->qubit_index_a);
	Qubit* qubit_b = model->get_qubit(this->qubit_index_b);
	
	QubitPosition* pos_a = qubit_a->get_position();
	QubitPosition* pos_b = qubit_b->get_position();
	
	int origin_a_i = pos_a->get_i();
	int origin_a_j = pos_a->get_j();
	
	int origin_b_i = pos_b->get_i();
	int origin_b_j = pos_b->get_j();
	
	int origin_left_j, origin_right_j;
	if (origin_a_j < origin_b_j) {
		origin_left_j = origin_a_j;
		origin_right_j = origin_b_j;
	} else {
		origin_left_j = origin_b_j;
		origin_right_j = origin_a_j;
	}
	
	naxos::NsProblemManager* pm = model->get_problem_manager();
	
	// Sites (reserve)
	naxos::NsIntVar* site_a = model->get_position_qubits_constraint(origin_a_i, origin_a_j);
	pm->add(*site_a == this->qubit_index_a);

	naxos::NsIntVar* site_b = model->get_position_qubits_constraint(origin_b_i, origin_b_j);
	pm->add(*site_b == this->qubit_index_b);

	// Barriers
	naxos::NsIntVar* middle_barrier = model->get_v_line_constraint(origin_left_j);
	pm->add(*middle_barrier == 1);

	if (origin_a_i - 1 >= 0) {
		naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(origin_a_i - 1);
		pm->add(*bottom_barrier == 0);
	}
	if (origin_a_i <= m - 2) {
		naxos::NsIntVar* top_barrier = model->get_h_line_constraint(origin_a_i);
		pm->add(*top_barrier == 0);
	}
	if (origin_left_j - 1 >= 0) {
		naxos::NsIntVar* left_barrier = model->get_v_line_constraint(origin_left_j - 1);
		pm->add(*left_barrier == 0);
	}
	if (origin_right_j <= n - 2) {
		naxos::NsIntVar* right_barrier = model->get_v_line_constraint(origin_right_j);
		pm->add(*right_barrier == 0);
	}

	// Qubit lines
	naxos::NsIntVar* left_line = model->get_d_line_constraint(origin_a_j - origin_a_i);
	naxos::NsIntVar* right_line = model->get_d_line_constraint(origin_b_j - origin_b_i);
	pm->add(*left_line == *right_line);
}

void CPhase::execute(CrossbarModel* model, int curr_cycle, bool with_animation, int speed) {
	// Evolve at the end by default
	if (this->is_cycle(curr_cycle, 2)) {
		model->evolve(this->get_involved_qubits());
	}
}
