#include "SqSwap.h"

Measurement::Measurement(int qubit_id, int ancilla_id, int line_number) {
	this->qubit_id = qubit_id;
	this->ancilla_id = ancilla_id;
	this->line_number = line_number;
}

void Measurement::add_static_constraints(CrossbarModel* model) {
	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_id);
	QubitPosition* pos = qubit->get_position();
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	naxos::NsProblemManager* pm = model->get_problem_manager();
	
	// 0. Check if an ancilla is adjacent to the qubit
	Qubit* ancilla;
	if (origin_j > 0 && !model->get_qubits(origin_i, origin_j - 1).empty()) {
		ancilla = model->get_qubit(model->get_qubits(origin_i, origin_j - 1).front());
	} else if (origin_j < n -1 && !model->get_qubits(origin_i, origin_j + 1).empty()) {
		ancilla = model->get_qubit(model->get_qubits(origin_i, origin_j + 1).front());
	} else {
		throw std::runtime_error("Conflict: There is no ancilla qubit near the target qubit");
	}
	
	if (!ancilla->get_is_ancillary()) {
		throw std::runtime_error("Conflict: qubit adjacent to target is not an ancilla qubit");
	}
	
	QubitPosition* pos_ancilla = ancilla->get_position();
	int origin_ancilla_i = pos_ancilla->get_i();
	int origin_ancilla_j = pos_ancilla->get_j();
	
	// 1. Ancilla must be in a known state |0> or |1>
	if (origin_ancilla_j % 2 == 0 && ancilla->get_state()->get_alpha() == 1) {
		throw std::runtime_error("Conflict: ancilla qubit must be in state |1>");
	} else if (origin_ancilla_j % 2 == 1 && ancilla->get_state()->get_beta() == 1) {
		throw std::runtime_error("Conflict: ancilla qubit must be in state |0>");
	} else {
		throw std::runtime_error("Conflict: ancilla qubit is in an unknown state");
	}
	
	// 2. Any of the sites vertically adjacent to the target should be empty
	if (origin_i > 0 && !model->get_qubits(origin_i - 1, origin_j).empty()
		&& !model->get_qubits(origin_i + 1, origin_).empty()) {
		throw std::runtime_error("Conflict: The site vertically adjacent to the target sites are not empty");
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

void Measurement::add_dynamic_constraints(CrossbarModel* model) {

}

void Measurement::execute(CrossbarModel* model, bool with_animation, int speed) {

	// TODO
	
}
