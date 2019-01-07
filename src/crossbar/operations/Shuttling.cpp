#include "Shuttling.h"

Shuttling::Shuttling(int direction, int qubit_id, int line_number) {
	this->direction = direction;
	this->qubit_id = qubit_id;
	this->line_number = line_number;
}

void Shuttling::add_static_constraints(CrossbarModel* model) {
	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_id);
	QubitPosition* pos = qubit->get_position();
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	naxos::NsProblemManager* pm = model->get_problem_manager();
	
	// TODO: handle out of bound in crossbar
	
	// Qubit constraints
	// 1. Empty space in left or right
	if (this->direction == Shuttling::DIR_UP) {
		// Top
		
		// Handle out of bound in crossbar
		if (origin_i + 1 >= m) {
			throw std::runtime_error("Conflict: Shuttling out of bounds");
		}
		
		// 1. Empty spot
		if (origin_i + 1 < m) {
			naxos::NsIntVar* top_site = model->get_position_qubits_constraint(origin_i + 1, origin_j);
			pm->add(*top_site == 0);
		}
		
		// 2. No two qubits in the same column
		for (int j = 0; j < n; j++) {
			if (j == origin_j) continue;
			naxos::NsIntVar* top_site = model->get_position_qubits_constraint(origin_i + 1, j);
			naxos::NsIntVar* bottom_site = model->get_position_qubits_constraint(origin_i, j);
			pm->add(NsIfThen(*top_site == 1, *bottom_site == 0));
			pm->add(NsIfThen(*bottom_site == 1, *top_site == 0));
		}
		
		// 3. Barriers
		
		// Left barrier
		if (origin_j > 0) {
			naxos::NsIntVar* left_barrier = model->get_v_line_constraint(origin_j - 1);
			pm->add(*left_barrier == 0);
		}

		// Right barrier
		if (origin_j < n - 1) {
			naxos::NsIntVar* right_barrier = model->get_v_line_constraint(origin_j);
			pm->add(*right_barrier == 0);
		}
		
		// Top barrier
		if (origin_i < m - 2) {
			naxos::NsIntVar* top_most_barrier = model->get_h_line_constraint(origin_i + 1);
			pm->add(*top_most_barrier == 0);
		}
		
		// Bottom barrier
		if (origin_i > 0) {
			naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(origin_i - 1);
			pm->add(*bottom_barrier == 0);
		}
	} else if (this->direction == Shuttling::DIR_DOWN) {
		// Down
		
		// Handle out of bound in crossbar
		if (origin_i - 1 < 0) {
			throw std::runtime_error("Conflict: Shuttling out of bounds");
		}
		
		// 1. Empty spot
		if (origin_i - 1 >= 0) {
			naxos::NsIntVar* bottom_site = model->get_position_qubits_constraint(origin_i - 1, origin_j);
			pm->add(*bottom_site == 0);
		}
		
		// 2. Two qubits in the same column
		for (int j = 0; j < n; j++) {
			if (j == origin_j) continue;
			naxos::NsIntVar* top_site = model->get_position_qubits_constraint(origin_i, j);
			naxos::NsIntVar* bottom_site = model->get_position_qubits_constraint(origin_i - 1, j);
			pm->add(NsIfThen(*top_site == 1, *bottom_site == 0));
			pm->add(NsIfThen(*bottom_site == 1, *top_site == 0));
		}
		
		// 3. Barriers
		
		// Left barrier
		if (origin_j > 0) {
			naxos::NsIntVar* left_barrier = model->get_v_line_constraint(origin_j - 1);
			pm->add(*left_barrier == 0);
		}

		// Right barrier
		if (origin_j < n - 1) {
			naxos::NsIntVar* right_barrier = model->get_v_line_constraint(origin_j);
			pm->add(*right_barrier == 0);
		}
		
		// Top barrier
		if (origin_i < m - 1) {
			naxos::NsIntVar* top_barrier = model->get_h_line_constraint(origin_i);
			pm->add(*top_barrier == 0);
		}
		
		// Bottom barrier
		if (origin_i > 1) {
			naxos::NsIntVar* bottom_most_barrier = model->get_h_line_constraint(origin_i - 2);
			pm->add(*bottom_most_barrier == 0);
		}	
	} else if (this->direction == Shuttling::DIR_LEFT) {
		// Left
		
		// Handle out of bound in crossbar
		if (origin_j - 1 < 0) {
			throw std::runtime_error("Conflict: Shuttling out of bounds");
		}
		
		// 1. Empty spot
		if (origin_j - 1 >= 0) {
			naxos::NsIntVar* left_site = model->get_position_qubits_constraint(origin_i,  origin_j - 1);
			pm->add(*left_site == 0);
		}
		
		// 2. Two qubits in the same row
		for (int i = 0; i < m; i++) {
			if (i == origin_i) continue;
			naxos::NsIntVar* left_site = model->get_position_qubits_constraint(i, origin_j - 1);
			naxos::NsIntVar* right_site = model->get_position_qubits_constraint(i, origin_j);
			pm->add(NsIfThen(*left_site == 1, *right_site == 0));
			pm->add(NsIfThen(*right_site == 1, *left_site == 0));
		}
		
		// 3. Barrier
		
		// Top barrier
		if (origin_i < n - 1) {
			naxos::NsIntVar* top_barrier = model->get_h_line_constraint(origin_i);
			pm->add(*top_barrier == 0);
		}

		// Bottom barrier
		if (origin_i > 0) {
			naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(origin_i - 1);
			pm->add(*bottom_barrier == 0);
		}
		
		// Left barrier
		if (origin_j > 1) {
			naxos::NsIntVar* left_most_barrier = model->get_v_line_constraint(origin_j - 2);
			pm->add(*left_most_barrier == 0);
		}
		
		// Right barrier
		if (origin_j < n - 1) {
			naxos::NsIntVar* right_barrier = model->get_v_line_constraint(origin_j);
			pm->add(*right_barrier == 0);
		}
	} else if (this->direction == Shuttling::DIR_RIGHT) {
		// Right
		
		// Handle out of bound in crossbar
		if (origin_j + 1 >= n) {
			throw std::runtime_error("Conflict: Shuttling out of bounds");
		}
		
		// 1. Empty spot
		if (origin_j + 1 < n) {
			naxos::NsIntVar* right_site = model->get_position_qubits_constraint(origin_i, origin_j + 1);
			pm->add(*right_site == 0);
		}
		
		// 2. Two qubits in the same row
		for (int i = 0; i < m; i++) {
			if (i == origin_i) continue;
			naxos::NsIntVar* left_site = model->get_position_qubits_constraint(i, origin_j);
			naxos::NsIntVar* right_site = model->get_position_qubits_constraint(i, origin_j + 1);
			pm->add(NsIfThen(*left_site == 1, *right_site == 0));
			pm->add(NsIfThen(*right_site == 1, *left_site == 0));
		}
		
		// 3. Barrier
		
		// Top barrier
		if (origin_i < n - 1) {
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
			pm->add(*left_barrier == 0);
		}

		// Right barrier
		if (origin_j < n - 2) {
			naxos::NsIntVar* right_most_barrier = model->get_v_line_constraint(origin_j + 1);
			pm->add(*right_most_barrier == 0);
		}
	} else {
		// throw error
		throw std::runtime_error("Invalid shuttling direction");
	}	
}

void Shuttling::add_dynamic_constraints(CrossbarModel* model) {
	// TODO
	
}

void Shuttling::execute(CrossbarModel* model, bool with_animation, int speed) {
	Qubit* qubit = model->get_qubit(this->qubit_id);
	QubitPosition* pos = qubit->get_position();
	
	// TODO: validate if we can shuttle
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	int dest_i, dest_j;
	double waiting_seconds = this->get_waiting_seconds(speed);
	
	switch (this->direction) {
		case Shuttling::DIR_UP:
			dest_i = pos->get_i() + 1;
			dest_j = pos->get_j();
			
			// Vertical barrier down
			model->toggle_h_line(origin_i);
			if (with_animation) this->wait(waiting_seconds);

			// Apply difference in QL voltages
			model->apply_diff_ql(origin_i, origin_j, dest_i, dest_j);
			model->evolve();
			if (with_animation) this->wait(waiting_seconds);

			// Vertical barrier up
			model->toggle_h_line(origin_i);
			if (with_animation) this->wait(waiting_seconds);
			break;
		case Shuttling::DIR_DOWN:
			dest_i = pos->get_i() - 1;
			dest_j = pos->get_j();
			
			// Vertical barrier down
			model->toggle_h_line(origin_i - 1);
			if (with_animation) this->wait(waiting_seconds);

			// Apply difference in QL voltages
			model->apply_diff_ql(origin_i, origin_j, dest_i, dest_j);
			model->evolve();
			if (with_animation) this->wait(waiting_seconds);

			// Vertical barrier up
			model->toggle_h_line(origin_i - 1);
			if (with_animation) this->wait(waiting_seconds);
			break;
		case Shuttling::DIR_LEFT:
			dest_i = pos->get_i();
			dest_j = pos->get_j() - 1;
			
			// Vertical barrier down
			model->toggle_v_line(origin_j - 1);
			if (with_animation) this->wait(waiting_seconds);

			// Apply difference in QL voltages
			model->apply_diff_ql(origin_i, origin_j, dest_i, dest_j);
			model->evolve();
			if (with_animation) this->wait(waiting_seconds);

			// Vertical barrier up
			model->toggle_v_line(origin_j - 1);
			if (with_animation) this->wait(waiting_seconds);
			break;
		case Shuttling::DIR_RIGHT:
			dest_i = pos->get_i();
			dest_j = pos->get_j() + 1;
			
			// Vertical barrier down
			model->toggle_v_line(origin_j);
			if (with_animation) this->wait(waiting_seconds);

			// Apply difference in QL voltages
			model->apply_diff_ql(origin_i, origin_j, dest_i, dest_j);
			model->evolve();
			if (with_animation) this->wait(waiting_seconds);

			// Vertical barrier up
			model->toggle_v_line(origin_j);
			if (with_animation) this->wait(waiting_seconds);
			break;
		default:
			throw std::runtime_error("Undefined shuttling instruction");
			break;
	}
}
