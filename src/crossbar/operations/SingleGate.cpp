#include "SingleGate.h"

SingleGate::SingleGate(int qubit_id, int line_number) {
	this->qubit_id = qubit_id;
	this->line_number = line_number;
}

void SingleGate::add_static_constraints(CrossbarModel* model) {
	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_id);
	QubitPosition* pos = qubit->get_position();
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	int left_j;
	if (origin_j - 1 >= 0) {
		// Left
		left_j = origin_j - 1;
	} else {
		left_j = origin_j;
	}
	int right_j;
	if (origin_j + 1 <= n - 1) {
		// Right
		right_j = origin_j + 1;
	} else {
		right_j = origin_j;
	}
	
	naxos::NsProblemManager* pm = model->get_problem_manager();
	
	// Define k direction
	naxos::NsIntVar* k_direction = new naxos::NsIntVar(*pm, -1, 1);
	pm->add(*k_direction != 0);
	
	// 1. Empty destination site
	naxos::NsIntVar* left_site = model->get_position_qubits_constraint(origin_i, left_j);
	naxos::NsIntVar* right_site = model->get_position_qubits_constraint(origin_i, right_j);
	pm->add(NsIfThen(*k_direction == SingleGate::K_DIR_LEFT, *left_site == 0));
	pm->add(NsIfThen(*k_direction == SingleGate::K_DIR_RIGHT, *right_site == 0));
	
	// 2. there are two qubits in the same row
	for (int i = 0; i < m; i++) {
		if (i == origin_i) continue;
		naxos::NsIntVar* local_left_site = model->get_position_qubits_constraint(i, left_j);
		naxos::NsIntVar* local_center_site = model->get_position_qubits_constraint(i, origin_j);
		naxos::NsIntVar* local_right_site = model->get_position_qubits_constraint(i, right_j);
		
		// If left
		pm->add(NsIfThen(
			*k_direction == SingleGate::K_DIR_LEFT,
			(*local_left_site == 0 || *local_center_site == 0)
		));
		
		// If right
		pm->add(NsIfThen(
			*k_direction == SingleGate::K_DIR_RIGHT,
			(*local_center_site == 0 || *local_right_site == 0)
		));
	}
	
	// 3. Barrier constraints
	
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
	if (origin_j > 0) {
		naxos::NsIntVar* left_barrier = model->get_v_line_constraint(origin_j - 1);
		pm->add(NsIfThen(*k_direction == SingleGate::K_DIR_RIGHT, *left_barrier == 0));
	}
	if (origin_j > 1) {
		naxos::NsIntVar* left_most_barrier = model->get_v_line_constraint(origin_j - 2);
		pm->add(NsIfThen(*k_direction == SingleGate::K_DIR_LEFT, *left_most_barrier == 0));
	}
	
	// Right barrier
	if (origin_j < n - 1) {
		naxos::NsIntVar* right_barrier = model->get_v_line_constraint(origin_j);
		pm->add(NsIfThen(*k_direction == SingleGate::K_DIR_LEFT, *right_barrier == 0));
	}
	if (origin_j < n - 2) {
		naxos::NsIntVar* right_most_barrier = model->get_v_line_constraint(origin_j + 1);
		pm->add(NsIfThen(*k_direction == SingleGate::K_DIR_RIGHT, *right_most_barrier == 0));
	}
}

void SingleGate::add_dynamic_constraints(CrossbarModel* model) {

}

void SingleGate::execute(CrossbarModel* model, bool with_animation, int speed) {
	// Qubit info
	Qubit* qubit = model->get_qubit(this->qubit_id);
	QubitPosition* pos = qubit->get_position();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	int dest_j;
	int v_barrier;
	if (origin_j - 1 >= 0  && model->get_qubits(origin_i, origin_j - 1).empty()) {
		// Left
		dest_j = origin_j - 1;
		v_barrier = origin_j - 1;
	} else if (origin_j + 1 >= 0  && model->get_qubits(origin_i, origin_j + 1).empty()) {
		// Right
		dest_j = origin_j + 1;
		v_barrier = origin_j;
	} else {
		throw std::runtime_error("No adjacent site empty. Can not execute one-qubit gate");
	}
	
	// Use settings
	double waiting_seconds = this->get_waiting_seconds(speed);
	
	// Apply global operation
	model->toggle_wave(origin_j % 2 == 0);
	if (with_animation) this->wait(waiting_seconds);
	// TODO: Wait rotation
	model->toggle_wave(origin_j % 2 == 0);
	if (with_animation) this->wait(waiting_seconds);
	
	// Vertical barrier down
	model->toggle_v_line(v_barrier);
	if (with_animation) this->wait(waiting_seconds);

	// Apply difference in QL voltages
	model->apply_diff_ql(origin_i, origin_j, origin_i, dest_j);
	model->evolve();
	if (with_animation) this->wait(waiting_seconds);

	// Vertical barrier up
	model->toggle_v_line(v_barrier);
	if (with_animation) this->wait(waiting_seconds);
	
	// Apply inverse global operation
	model->toggle_wave(origin_j % 2 == 0);
	if (with_animation) this->wait(waiting_seconds);
	// TODO: Wait rotation
	model->toggle_wave(origin_j % 2 == 0);
	if (with_animation) this->wait(waiting_seconds);
	
	// Vertical barrier down
	model->toggle_v_line(v_barrier);
	if (with_animation) this->wait(waiting_seconds);
	
	// Apply difference in QL voltages
	model->apply_diff_ql(origin_i, dest_j, origin_i, origin_j);
	model->evolve();
	if (with_animation) this->wait(waiting_seconds);
	
	// Vertical barrier up
	model->toggle_v_line(v_barrier);
	if (with_animation) this->wait(waiting_seconds);
}