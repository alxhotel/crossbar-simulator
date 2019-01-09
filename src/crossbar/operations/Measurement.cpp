#include "Measurement.h"

Measurement::Measurement(int qubit_id, int line_number) {
	this->qubit_id = qubit_id;
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
		auto first = *model->get_qubits(origin_i, origin_j - 1).begin();
		ancilla = model->get_qubit(first);
	} else if (origin_j < n -1 && !model->get_qubits(origin_i, origin_j + 1).empty()) {
		auto first = *model->get_qubits(origin_i, origin_j + 1).begin();
		ancilla = model->get_qubit(first);
	} else {
		throw std::runtime_error("Conflict: There is no ancilla qubit near the target qubit");
	}
	
	if (!ancilla->get_is_ancillary()) {
		throw std::runtime_error("Conflict: qubit adjacent to target is not an ancilla qubit");
	}
	
	QubitPosition* pos_ancilla = ancilla->get_position();
	int origin_ancilla_i = pos_ancilla->get_i();
	int origin_ancilla_j = pos_ancilla->get_j();
	
	int origin_left_j, origin_right_j;
	if (origin_i < origin_ancilla_j) {
		origin_left_j = origin_i;
		origin_right_j = origin_ancilla_j;
	} else {
		origin_left_j = origin_ancilla_j;
		origin_right_j = origin_i;
	}
	
	// 1. Ancilla must be in a known state |0> or |1>
	std::complex<double> complex_one = 1;
	if (!(origin_ancilla_j % 2 == 0 && ancilla->get_state()->get_beta() == complex_one)
		&& !(origin_ancilla_j % 2 == 1 && ancilla->get_state()->get_alpha() == complex_one)) {
		throw std::runtime_error("Conflict: ancillary qubit must be in |0> or |1> state");
	}
	
	// 2. Any of the sites vertically adjacent to the target should be empty
	int empty_site_i;
	if (origin_i > 0 && model->get_qubits(origin_i - 1, origin_j).empty()) {
		empty_site_i = origin_i - 1;
	} else if (origin_i < m - 1 && model->get_qubits(origin_i + 1, origin_j).empty()) {
		empty_site_i = origin_i + 1;
	} else {
		throw std::runtime_error("Conflict: The sites vertically adjacent to the target sites are not empty");
	}
	
	// 2. No two qubits in the same row (FOR FIRST PHASE)
	for (int i = 0; i < m; i++) {
		if (i == origin_i) continue;
		naxos::NsIntVar* target_site = model->get_position_qubits_constraint(i, origin_j);
		naxos::NsIntVar* ancilla_site = model->get_position_qubits_constraint(i, origin_ancilla_j);
		pm->add(*target_site == 0 || *ancilla_site == 0);
	}
	
	// 2. No two qubits in the same column (FOR SECOND PHASE)
	for (int j = 0; j < n; j++) {
		if (j == origin_j) continue;
		naxos::NsIntVar* target_site = model->get_position_qubits_constraint(origin_i, j);
		naxos::NsIntVar* empty_site = model->get_position_qubits_constraint(empty_site_i, j);
		pm->add(*target_site == 0 || *empty_site == 0);
	}

	// 3. Barriers

	// TODO: add barriers for second phase
	
	// Top barrier
	if (origin_i < m - 1) {
		naxos::NsIntVar* top_barrier = model->get_h_line_constraint(origin_i);
		pm->add(*top_barrier == 0);
	}

	// Bottom barrier
	if (origin_i > 0) {
		naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(origin_i - 1);
		pm->add(*bottom_barrier == 0);
	}
	
	// Left barrier
	if (origin_left_j > 0) {
		naxos::NsIntVar* left_most_barrier = model->get_v_line_constraint(origin_left_j - 1);
		pm->add(*left_most_barrier == 0);
	}

	// Right barrier
	if (origin_right_j < n - 1) {
		naxos::NsIntVar* right_most_barrier = model->get_v_line_constraint(origin_right_j);
		pm->add(*right_most_barrier == 0);
	}
}

void Measurement::add_dynamic_constraints(CrossbarModel* model) {

}

void Measurement::execute(CrossbarModel* model, bool with_animation, int speed) {
	// Qubit info
	Qubit* qubit = model->get_qubit(this->qubit_id);
	QubitPosition* pos = qubit->get_position();
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	// Get ancilla
	Qubit* ancilla;
	int v_barrier;
	if (origin_j > 0 && !model->get_qubits(origin_i, origin_j - 1).empty()) {
		auto first = *model->get_qubits(origin_i, origin_j - 1).begin();
		ancilla = model->get_qubit(first);
		v_barrier = origin_j - 1;
	} else if (origin_j < n - 1 && !model->get_qubits(origin_i, origin_j + 1).empty()) {
		auto first = *model->get_qubits(origin_i, origin_j + 1).begin();
		ancilla = model->get_qubit(first);
		v_barrier = origin_j;
	} else {
		throw std::runtime_error("There is no ancilla qubit near the target qubit");
	}
	QubitPosition* pos_ancilla = ancilla->get_position();

	int origin_ancilla_i = pos_ancilla->get_i();
	int origin_anacilla_j = pos_ancilla->get_j();
	
	// Use settings
	double waiting_seconds = this->get_waiting_seconds(speed);
	
	// FIRST PHASE
	
	// Vertical barrier down
	model->toggle_v_line(v_barrier);
	if (with_animation) this->wait(waiting_seconds);

	// Apply difference in QL voltages
	model->apply_diff_ql(origin_i, origin_j, origin_ancilla_i, origin_anacilla_j);
	//model->evolve();
	if (with_animation) this->wait(waiting_seconds);

	// Vertical barrier up
	model->toggle_v_line(v_barrier);
	if (with_animation) this->wait(waiting_seconds);
	
	// SECOND PHASE
	
	// Get empty site
	int empty_site_i;
	int h_barrier;
	if (origin_i > 0 && model->get_qubits(origin_i - 1, origin_j).empty()) {
		empty_site_i = origin_i - 1;
		h_barrier = origin_i - 1;
	} else if (origin_i < m - 1 && model->get_qubits(origin_i + 1, origin_j).empty()) {
		empty_site_i = origin_i + 1;
		h_barrier = origin_i;
	} else {
		throw std::runtime_error("Conflict: The sites vertically adjacent to the target sites are not empty");
	}
	
	// Horizontal barrier down
	model->toggle_h_line(h_barrier);
	if (with_animation) this->wait(waiting_seconds);
	
	// Alternate QL lines
	for (int a = 0; a < 5; a++) {
		if (a % 2 == 0) {
			model->apply_diff_ql(origin_i, origin_j, empty_site_i, origin_j);
		} else {
			model->apply_diff_ql(empty_site_i, origin_j, origin_i, origin_j);
		}
		model->evolve();
		if (with_animation) this->wait(waiting_seconds);
	}
	model->apply_diff_ql(empty_site_i, origin_j, origin_i, origin_j);
	model->evolve();
	if (with_animation) this->wait(waiting_seconds);
	
	// Horizontal barrier up
	model->toggle_h_line(h_barrier);
	if (with_animation) this->wait(waiting_seconds);
}
