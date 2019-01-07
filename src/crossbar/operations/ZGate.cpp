#include "ZGate.h"

ZGate::ZGate(int qubit_id, int line_number) {
	this->qubit_id = qubit_id;
	this->line_number = line_number;
}

void ZGate::add_static_constraints(CrossbarModel* model) {
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
	if (origin_j + 1 < n) {
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
	pm->add(NsIfThen(*k_direction == ZGate::K_DIR_LEFT, *left_site == 0));
	pm->add(NsIfThen(*k_direction == ZGate::K_DIR_RIGHT, *right_site == 0));
	
	// 2. there are two qubits in the same row
	for (int i = 0; i < m; i++) {
		if (i == origin_i) continue;
		naxos::NsIntVar* local_left_site = model->get_position_qubits_constraint(i, left_j);
		naxos::NsIntVar* local_center_site = model->get_position_qubits_constraint(i, origin_j);
		naxos::NsIntVar* local_right_site = model->get_position_qubits_constraint(i, right_j);
		
		// If left
		pm->add(NsIfThen(
			*k_direction == ZGate::K_DIR_LEFT,
			(*local_left_site == 0 || *local_center_site == 0)
		));
		
		// If right
		pm->add(NsIfThen(
			*k_direction == ZGate::K_DIR_RIGHT,
			(*local_center_site == 0 || *local_right_site == 0)
		));
	}
	
	// 3. Barrier constraints
	
	// Top barrier
	if (pos->get_i() < m - 1) {
		naxos::NsIntVar* top_barrier = model->get_h_line_constraint(pos->get_i());
		pm->add(*top_barrier == 0);
	}
	
	// Bottom barrier
	if (pos->get_i() > 0) {
		naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(pos->get_i() - 1);
		pm->add(*bottom_barrier == 0);
	}
	
	// Left barrier
	if (pos->get_j() > 0) {
		naxos::NsIntVar* left_barrier = model->get_v_line_constraint(pos->get_j() - 1);
		pm->add(NsIfThen(*k_direction == ZGate::K_DIR_RIGHT, *left_barrier == 0));
	}
	if (pos->get_j() > 1) {
		naxos::NsIntVar* left_most_barrier = model->get_v_line_constraint(pos->get_j() - 2);
		pm->add(NsIfThen(*k_direction == ZGate::K_DIR_LEFT, *left_most_barrier == 0));
	}
	
	// Right barrier
	if (pos->get_j() < n - 1) {
		naxos::NsIntVar* right_barrier = model->get_v_line_constraint(pos->get_j());
		pm->add(NsIfThen(*k_direction == ZGate::K_DIR_LEFT, *right_barrier == 0));
	}
	if (pos->get_j() < n - 2) {
		naxos::NsIntVar* right_most_barrier = model->get_v_line_constraint(pos->get_j() + 1);
		pm->add(NsIfThen(*k_direction == ZGate::K_DIR_RIGHT, *right_most_barrier == 0));
	}
}

void ZGate::add_dynamic_constraints(CrossbarModel* model) {
	// TODO
	
	// Qubits
	
	// QL lines
	
	// Barriers
	
}
	
void ZGate::execute(CrossbarModel* model, bool with_animation, int speed) {
	Qubit* qubit = model->get_qubit(this->qubit_id);
	QubitPosition* pos = qubit->get_position();
	
	// Try by shuttling...
	int origin_j, dest_j;
	int v_barrier;
	if (pos->get_j() - 1 >= 0  && model->get_qubits(pos->get_i(), pos->get_j() - 1).empty()) {
		// Left
		origin_j = pos->get_j();
		dest_j = origin_j - 1;
		v_barrier = origin_j - 1;
	} else if (pos->get_j() + 1 >= 0  && model->get_qubits(pos->get_i(), pos->get_j() + 1).empty()) {
		// Right
		origin_j = pos->get_j();
		dest_j = origin_j + 1;
		v_barrier = origin_j;
	} else {
		// TODO If not, use global operation?
		throw std::runtime_error("Can not execute Z gate");
	}
	
	// Use settings
	double waiting_seconds = this->get_waiting_seconds(speed);
	
	// Vertical barrier down
	model->toggle_v_line(v_barrier);
	if (with_animation) this->wait(waiting_seconds);

	// Apply difference in QL voltages
	model->apply_diff_ql(pos->get_i(), pos->get_j(), pos->get_i(), dest_j);
	model->evolve();
	if (with_animation) this->wait(waiting_seconds);

	// TODO: Wait 100 ns?
	
	// Apply difference in QL voltages
	model->apply_diff_ql(pos->get_i(), dest_j, pos->get_i(), origin_j);
	model->evolve();
	if (with_animation) this->wait(waiting_seconds);
	
	// Vertical barrier up
	model->toggle_v_line(v_barrier);
	if (with_animation) this->wait(waiting_seconds);
	
	// TODO: apply z gate with latency to model and qubit state
}
