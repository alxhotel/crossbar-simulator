#include "SingleGate.h"

SingleGate::SingleGate(std::string gate, int direction, int qubit_index, int line_number) : Operation(0, 40) {
	this->gate = gate;
	this->direction = direction;
	this->qubit_index = qubit_index;
	this->line_number = line_number;
}

void SingleGate::check_static_constraints(CrossbarModel* model) {
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	int left_j;
	if (origin_j - 1 >= 0) {
		left_j = origin_j - 1;
	} else {
		left_j = origin_j;
	}
	int right_j;
	if (origin_j + 1 <= n - 1) {
		right_j = origin_j + 1;
	} else {
		right_j = origin_j;
	}
	
	// Common barriers
	
	// Top barrier
	for (int i = 0; i < m - 1; i++) {
		if (model->is_h_barrier_down(i)) {
			throw std::runtime_error(
				std::string("Conflict: The horizontal barrier ")
				+ std::to_string(i)
				+ " is lowered"
			);
		}
	}
	
	// Bottom barrier
	for (int j = 0; j < n - 1; j++) {
		if (model->is_v_barrier_down(j)) {
			throw std::runtime_error(
				std::string("Conflict: The vertical barrier ")
				+ std::to_string(j)
				+ " is lowered"
			);
		}
	}
	
	// 1. Empty destination site
	// 2. there are two qubits in the same row
	// 3. Barrier constraints
	/*if (this->direction == SingleGate::DIR_LEFT) {
		this->check_left_side(model, origin_i, origin_j, left_j, right_j);
	} else if (this->direction == SingleGate::DIR_RIGHT) {
		this->check_right_side(model, origin_i, origin_j, left_j, right_j);
	} else {
		throw std::runtime_error("ERROR: SingleGate has an unknown direction");
	}*/
	
	/*try {
		this->check_left_side(model, origin_i, origin_j, left_j, right_j);
	} catch (std::runtime_error err_left) {
		try {
			this->check_right_side(model, origin_i, origin_j, left_j, right_j);
		} catch (std::runtime_error err_right) {
			throw std::runtime_error("Conflict: SingleGate can not be done by shuttling left nor right");
		}
	}*/
}

void SingleGate::check_left_side(CrossbarModel* model, int origin_i, int origin_j, int left_j, int right_j) {
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
	/*if (origin_j > 1) {
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
	}*/
}

void SingleGate::check_right_side(CrossbarModel* model, int origin_i, int origin_j, int left_j, int right_j) {
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	// 1. Empty destination site
	std::set<int> right_site = model->get_qubits(origin_i, right_j);
	if (!right_site.empty()) {
		throw std::runtime_error(
			std::string("Conflict: the right adjacent site to ")
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
	/*if (origin_j > 0) {
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
	}*/
}

void SingleGate::add_dynamic_constraints(CrossbarModel* model, int curr_cycle) {
	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	naxos::NsProblemManager* pm = model->get_problem_manager();
	
	// Sites
	/*naxos::NsIntVar* originSite = model->get_position_qubits_constraint(origin_i, origin_j);
	pm->add(*originSite == this->qubit_index);

	naxos::NsIntVar* destinationSite = model->get_position_qubits_constraint(origin_i + 1, origin_j);
	pm->add(*destinationSite == this->qubit_index);*/
	
	// Barriers
	for (int k = 0; k < (n - 1); k++) {
		// Horizontal
		naxos::NsIntVar* h_barrier = model->get_h_line_constraint(k);
		pm->add(*h_barrier == 0);
	}
	
	for (int k = 0; k < (m - 1); k++) {
		// Vertical
		naxos::NsIntVar* v_barrier = model->get_v_line_constraint(k);
		pm->add(*v_barrier == 0);
	}
	
	// Qubit lines
	for (int k = -1 * (n - 1); k <= (m - 1); k++) {
		naxos::NsIntVar* d_line = model->get_d_line_constraint(k);
		pm->add(*d_line == 0);
	}
	
	// Wave
	naxos::NsIntVar* wave = model->get_wave_constraint();
	naxos::NsIntVar* wave_column = model->get_wave_column_constraint();
	pm->add(*wave == this->get_wave_hash());
	pm->add(*wave_column == (origin_j % 2));
}

void SingleGate::execute(CrossbarModel* model, int curr_cycle, bool with_animation, int speed) {
	// Qubit info
	/*Qubit* qubit = model->get_qubit(this->qubit_index);
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
	if (with_animation) this->wait(waiting_seconds);*/
}

int SingleGate::get_wave_hash() {
	return this->get_string_hash(this->gate);
}

int SingleGate::get_string_hash(const std::string& str) {
	int hash = 5381;
	for (size_t i = 0; i < str.size(); i++) {
		hash = 33 * hash + (unsigned char) str[i];
	}
	return hash;
}
