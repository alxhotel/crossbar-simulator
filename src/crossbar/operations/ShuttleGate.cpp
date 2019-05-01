#include "ShuttleGate.h"

ShuttleGate::ShuttleGate(int direction, int qubit_index, int line_number) : Operation(0, 20) {
	this->direction = direction;
	this->qubit_index = qubit_index;
	this->line_number = line_number;
}

void ShuttleGate::check_static_constraints(CrossbarModel* model) {
	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	int left_j;
	if (origin_j - 1 >= 0) {
		left_j = origin_j - 1;
	} else {
		left_j = origin_j;
	}
	int right_j;
	if (origin_j + 1 < n) {
		right_j = origin_j + 1;
	} else {
		right_j = origin_j;
	}
	
	// Common barriers
	
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
	if (origin_i > 0) {
		if (model->is_h_barrier_down(origin_i - 1)) {
			throw std::runtime_error(
				std::string("Conflict: The horizontal barrier ")
				+ std::to_string(origin_i - 1)
				+ " is lowered"
			);
		}
	}
	
	// 1. Empty destination site
	// 2. There are two qubits in the same row
	// 3. Barrier constraints
	if (this->direction == ShuttleGate::DIR_LEFT) {
		this->check_left_side(model, origin_i, origin_j, left_j, right_j);
	} else if (this->direction == ShuttleGate::DIR_RIGHT) {
		this->check_right_side(model, origin_i, origin_j, left_j, right_j);
	} else {
		throw std::runtime_error("ERROR: ShuttleGate has an unknown direction");
	}
}

void ShuttleGate::check_left_side(CrossbarModel* model, int origin_i, int origin_j, int left_j, int right_j) {
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	// 1. Empty destination site
	std::set<int> left_site = model->get_qubits(origin_i, left_j);
	if (!left_site.empty()) {
		throw std::runtime_error(
			std::string("Conflict: the left adjacent site to ")
			+ "(" + std::to_string(origin_i) + ", " + std::to_string(origin_j) + ")"
			+ " is occupied"
		);
	}
	
	// 2. there are two qubits in the same row
	/*for (int i = 0; i < m; i++) {
		if (i == origin_i) continue;
		std::set<int> local_left_site = model->get_qubits(i, left_j);
		std::set<int> local_center_site = model->get_qubits(i, origin_j);

		if (!local_left_site.empty() && !local_center_site.empty()) {
			throw std::runtime_error(
				"Conflict: Two adjacent qubits in the same row " + std::to_string(i)
			);
		}
	}*/
	
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
}

void ShuttleGate::check_right_side(CrossbarModel* model, int origin_i, int origin_j, int left_j, int right_j) {
	// Get params
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	// 1. Empty destination site
	std::set<int> right_site = model->get_qubits(origin_i, right_j);
	if (!right_site.empty()) {
		throw std::runtime_error(
			std::string("Conflict:the right adjacent site to ")
			+ "(" + std::to_string(origin_i) + ", " + std::to_string(origin_j) + ")"
			+ " is occupied"
		);
	}
	
	// 2. there are two qubits in the same row
	/*for (int i = 0; i < m; i++) {
		if (i == origin_i) continue;
		std::set<int> local_center_site = model->get_qubits(i, origin_j);
		std::set<int> local_right_site = model->get_qubits(i, right_j);
		
		if (!local_center_site.empty() && !local_right_site.empty()) {
			throw std::runtime_error(
				"Conflict: Two adjacent qubits in the same row " + std::to_string(i)
			);
		}
	}*/

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
}

void ShuttleGate::add_dynamic_constraints(CrossbarModel* model, int curr_cycle) {
	// Get params
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	int dest_j;
	int v_barrier;

	if (this->is_cycle(curr_cycle, 0)) {
		// Go there
		if (this->direction == ShuttleGate::DIR_LEFT) {
			// Left
			dest_j = origin_j - 1;
			v_barrier = origin_j - 1;
		} else if (this->direction == ShuttleGate::DIR_RIGHT) {
			// Right
			dest_j = origin_j + 1;
			v_barrier = origin_j;
		} else {
			throw std::runtime_error("Undefined ShuttleGate");
		}
	} else {
		// Go back
		if (this->direction == ShuttleGate::DIR_LEFT) {
			// Right
			dest_j = origin_j + 1;
			v_barrier = origin_j;
		} else if (this->direction == ShuttleGate::DIR_RIGHT) {
			// Left
			dest_j = origin_j - 1;
			v_barrier = origin_j - 1;
		} else {
			throw std::runtime_error("Undefined ShuttleGate");
		}
	}
	
	naxos::NsProblemManager* pm = model->get_problem_manager();
	
	// Sites
	naxos::NsIntVar* origin_site = model->get_position_qubits_constraint(origin_i, origin_j);
	pm->add(*origin_site == this->qubit_index);

	// Destination
	naxos::NsIntVar* destination_site = model->get_position_qubits_constraint(origin_i, dest_j);
	pm->add(*destination_site == this->qubit_index);

	// Barriers
	naxos::NsIntVar* middle_barrier = model->get_v_line_constraint(v_barrier);
	pm->add(*middle_barrier == 1);

	if (origin_i > 0) {
		naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(origin_i - 1);
		pm->add(*bottom_barrier == 0);
	}
	if (origin_i < n - 1) {
		naxos::NsIntVar* top_barrier = model->get_h_line_constraint(origin_i);
		pm->add(*top_barrier == 0);
	}
	if (v_barrier > 0) {
		naxos::NsIntVar* left_barrier = model->get_v_line_constraint(v_barrier - 1);
		pm->add(*left_barrier == 0);
	}
	if (v_barrier < n - 2) {
		naxos::NsIntVar* right_barrier = model->get_v_line_constraint(v_barrier + 1);
		pm->add(*right_barrier == 0);
	}

	// Qubit lines
	naxos::NsIntVar* origin_line = model->get_d_line_constraint(origin_j - origin_i);
	naxos::NsIntVar* dest_line = model->get_d_line_constraint(dest_j - origin_i);
	pm->add(*origin_line < *dest_line);
}
	
void ShuttleGate::execute(CrossbarModel* model, int curr_cycle, bool with_animation, int speed) {
	if (this->is_cycle(curr_cycle, 1)) {
		// Go there
		model->evolve(this->get_involved_qubits());
	} else if (this->is_cycle(curr_cycle, 2)) {
		// Come back
		model->evolve(this->get_involved_qubits());
	}
	
	// Get info
	/*Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	// Try by shuttling...
	int dest_j;
	int v_barrier;
	if (this->direction == ShuttleGate::DIR_LEFT) {
		// Left
		dest_j = origin_j - 1;
		v_barrier = origin_j - 1;
	} else if (this->direction == ShuttleGate::DIR_RIGHT) {
		// Right
		dest_j = origin_j + 1;
		v_barrier = origin_j;
	} else {
		throw std::runtime_error("Undefined ShuttleGate");
	}
	
	// Use settings
	double waiting_seconds = this->get_waiting_seconds(speed);
	
	// Vertical barrier down
	model->toggle_v_line(v_barrier);
	if (with_animation) this->wait(waiting_seconds);

	// Apply difference in QL voltages
	model->apply_diff_ql(origin_i, origin_j, origin_i, dest_j);
	model->evolve();
	if (with_animation) this->wait(waiting_seconds);

	// TODO: Wait 100 ns?
	
	// Apply difference in QL voltages
	model->apply_diff_ql(origin_i, dest_j, origin_i, origin_j);
	model->evolve();
	if (with_animation) this->wait(waiting_seconds);
	
	// Vertical barrier up
	model->toggle_v_line(v_barrier);
	if (with_animation) this->wait(waiting_seconds);*/
	
	// TODO: apply z gate with latency to model and qubit state
}
