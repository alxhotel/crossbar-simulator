#include "SqSwap.h"

SqSwap::SqSwap(int qubit_a_id, int qubit_b_id, int line_number) {
	this->qubit_a_id = qubit_a_id;
	this->qubit_b_id = qubit_b_id;
	this->line_number = line_number;
}

void SqSwap::add_static_constraints(CrossbarModel* model) {
	// Get info
	Qubit* qubit_a = model->get_qubit(this->qubit_a_id);
	Qubit* qubit_b = model->get_qubit(this->qubit_b_id);
	QubitPosition* pos_a = qubit_a->get_position();
	QubitPosition* pos_b = qubit_b->get_position();
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	int origin_a_i = pos_a->get_i();
	int origin_a_j = pos_a->get_j();
	int origin_b_i = pos_b->get_i();
	int origin_b_j = pos_b->get_j();
	
	int origin_top_i, origin_bottom_i;
	if (origin_b_i < origin_a_i) {
		origin_top_i = origin_a_i;
		origin_bottom_i = origin_b_i;
	} else {
		origin_top_i = origin_b_i;
		origin_bottom_i = origin_a_i;
	}
	
	naxos::NsProblemManager* pm = model->get_problem_manager();
	
	// 1. Both qubits in same column
	if (origin_a_j != origin_b_j) {
		throw std::runtime_error("Conflict: SqSwap between qubits in different columns");
	}
	if (abs(origin_a_i - origin_b_i) != 1) {
		throw std::runtime_error("Conflict: SqSwap between qubits too far away");
	}
	
	// 2. No two qubits in the same column
	for (int j = 0; j < n; j++) {
		if (j == origin_a_j) continue;
		naxos::NsIntVar* top_site = model->get_position_qubits_constraint(origin_top_i, j);
		naxos::NsIntVar* bottom_site = model->get_position_qubits_constraint(origin_bottom_i, j);
		pm->add(*top_site == 0 || *bottom_site == 0);
	}

	// 3. Barriers
	
	// Left barrier
	if (origin_a_j > 0) {
		naxos::NsIntVar* left_barrier = model->get_v_line_constraint(origin_a_j - 1);
		pm->add(*left_barrier == 0);
	}

	// Right barrier
	if (origin_a_j < n - 1) {
		naxos::NsIntVar* right_barrier = model->get_v_line_constraint(origin_a_j);
		pm->add(*right_barrier == 0);
	}

	// Top barrier
	if (origin_top_i < m - 1) {
		naxos::NsIntVar* top_most_barrier = model->get_h_line_constraint(origin_top_i);
		pm->add(*top_most_barrier == 0);
	}

	// Bottom barrier
	if (origin_bottom_i > 0) {
		naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(origin_bottom_i - 1);
		pm->add(*bottom_barrier == 0);
	}
}

void SqSwap::add_dynamic_constraints(CrossbarModel* model) {

}

void SqSwap::execute(CrossbarModel* model, bool with_animation, int speed) {

	// TODO
	
}
