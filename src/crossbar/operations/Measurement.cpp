#include "Measurement.h"

Measurement::Measurement(int ancilla_direction, int site_direction, int qubit_index, int line_number) : Operation(0, 80) {
	this->ancilla_direction = ancilla_direction;
	this->site_direction = site_direction;
	this->qubit_index = qubit_index;
	this->line_number = line_number;
}

void Measurement::check_static_constraints(CrossbarModel* model) {
	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	// 0. Check if an ancilla is adjacent to the qubit
	Qubit* ancilla;
	std::set<int> ancilla_site_qubits;
	if (this->ancilla_direction == DIR_ANCILLA_LEFT && origin_j > 0) {
		ancilla_site_qubits = model->get_qubits(origin_i, origin_j - 1);
	} else if (this->ancilla_direction == DIR_ANCILLA_RIGHT && origin_j < n - 1) {
		ancilla_site_qubits = model->get_qubits(origin_i, origin_j + 1);
	} else {
		throw std::runtime_error("ERROR: Measurement has an invalid ancilla direction");
	}
	
	if (ancilla_site_qubits.size() == 0) {
		throw std::runtime_error("Conflict: There is no ancilla in that direction");
	} else {
		auto ancilla_index = *ancilla_site_qubits.begin();
		ancilla = model->get_qubit(ancilla_index);
		
		if (!ancilla->get_is_ancillary()) {
			//throw std::runtime_error("Conflict: qubit adjacent to target is not an ancilla qubit");
		}
	}
	
	QubitPosition* pos_ancilla = ancilla->get_position();
	int origin_ancilla_j = pos_ancilla->get_j();
	
	int origin_left_j, origin_right_j;
	if (origin_j < origin_ancilla_j) {
		origin_left_j = origin_j;
		origin_right_j = origin_ancilla_j;
	} else {
		origin_left_j = origin_ancilla_j;
		origin_right_j = origin_j;
	}
	
	// 1. Ancilla must be in a known state |0> or |1>
	std::complex<double> complex_one = 1;
	if (!(origin_ancilla_j % 2 == 0 && ancilla->get_state()->get_beta() == complex_one)
		&& !(origin_ancilla_j % 2 == 1 && ancilla->get_state()->get_alpha() == complex_one)) {
		throw std::runtime_error("Conflict: ancillary qubit must be in |0> or |1> state");
	}
	
	// 2. Any of the sites vertically adjacent to the target should be empty
	int empty_site_i;
	if (this->site_direction == Measurement::DIR_SITE_UP && origin_i < m - 1) {
		empty_site_i = origin_i + 1;
	} else if (this->site_direction == Measurement::DIR_SITE_DOWN && origin_i > 0) {
		empty_site_i = origin_i - 1;
	} else {
		throw std::runtime_error("Conflict: Measurement has an invalid site direction");
	}
	
	if (!model->get_qubits(empty_site_i, origin_j).empty()) {
		throw std::runtime_error("Conflict: The site vertically adjacent to the measured qubit is not empty");
	}
	
	// 2. No two qubits in the same row (FOR FIRST PHASE)
	/*for (int i = 0; i < m; i++) {
		if (i == origin_i) continue;
		std::set<int> target_site = model->get_qubits(i, origin_j);
		std::set<int> ancilla_site = model->get_qubits(i, origin_ancilla_j);
		if (!target_site.empty() && !ancilla_site.empty()) {
			throw std::runtime_error(
				"Conflict: Two adjacent qubits in the same row " + std::to_string(i)
			);
		}
	}*/
	
	// 2. No two qubits in the same column (FOR SECOND PHASE)
	/*for (int j = 0; j < n; j++) {
		if (j == origin_j) continue;
		std::set<int> target_site = model->get_qubits(origin_i, j);
		std::set<int> empty_site = model->get_qubits(empty_site_i, j);
		if (!target_site.empty() && !empty_site.empty()) {
			throw std::runtime_error(
				"Conflict: Two adjacent qubits in the same column " + std::to_string(j)
			);
		}
	}*/

	// 3. Barriers

	// TODO: add barriers for second phase
	
	// Top barrier
	/*if (origin_i < m - 1) {
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
	}*/
}

void Measurement::add_dynamic_constraints(CrossbarModel* model, int curr_cycle) {
	// Get params
	int m, n;
	std::tie(m, n) = model->get_dimensions();

	// Get info
	Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	naxos::NsProblemManager* pm = model->get_problem_manager();
	
	if (curr_cycle >= -1 && curr_cycle <= 2) {
		// Shuttle to ancilla site

		int ancilla_origin_i = origin_i;
		int ancilla_origin_j;
		if (this->ancilla_direction == DIR_ANCILLA_LEFT) {
			ancilla_origin_j = origin_j - 1;
		} else if (this->ancilla_direction == DIR_ANCILLA_LEFT) {
			ancilla_origin_j = origin_j + 1;
		}
		
		// Sites
		naxos::NsIntVar* origin_site = model->get_position_qubits_constraint(origin_i, origin_j);
		pm->add(*origin_site == this->qubit_index);

		naxos::NsIntVar* ancilla_site = model->get_position_qubits_constraint(origin_i, ancilla_origin_j);
		pm->add(*ancilla_site == this->qubit_index);

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
		naxos::NsIntVar* origin_line = model->get_d_line_constraint(origin_j - origin_i);
		naxos::NsIntVar* ancilla_line = model->get_d_line_constraint(ancilla_origin_j - ancilla_origin_i);
		pm->add(*origin_line > *ancilla_line);
		
	} else if (curr_cycle <= 6) {
		// Simulate the QL wave
		if (curr_cycle % 2 == 0) {
			// Leave at origin
			
			int empty_site_i = origin_i;
			int empty_site_j = origin_j;
			int h_barrier;
			if (this->site_direction == Measurement::DIR_SITE_UP) {
				origin_i = empty_site_i - 1;
				h_barrier = origin_i;
			} else if (this->site_direction == Measurement::DIR_SITE_DOWN) {
				origin_i = empty_site_i + 1;
				h_barrier = origin_i - 1;
			}
			
			// Sites
			naxos::NsIntVar* origin_site = model->get_position_qubits_constraint(origin_i, origin_j);
			pm->add(*origin_site == this->qubit_index);

			naxos::NsIntVar* ancilla_site = model->get_position_qubits_constraint(empty_site_i, empty_site_j);
			pm->add(*ancilla_site == this->qubit_index);

			// Barriers
			naxos::NsIntVar* middle_barrier = model->get_h_line_constraint(h_barrier);
			pm->add(*middle_barrier == 1);

			if (h_barrier > 0) {
				naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(h_barrier - 1);
				pm->add(*bottom_barrier == 0);
			}
			if (h_barrier < n - 2) {
				naxos::NsIntVar* top_barrier = model->get_h_line_constraint(h_barrier + 1);
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
			naxos::NsIntVar* origin_line = model->get_d_line_constraint(origin_j - origin_i);
			naxos::NsIntVar* empty_line = model->get_d_line_constraint(empty_site_j - empty_site_i);
			pm->add(*origin_line > *empty_line);
			
		} else {
			// Shuttle to empty siten
			
			int empty_site_i;
			int empty_site_j = origin_j;
			int h_barrier;
			if (this->site_direction == Measurement::DIR_SITE_UP) {
				empty_site_i = origin_i + 1;
				h_barrier = origin_i;
			} else if (this->site_direction == Measurement::DIR_SITE_DOWN) {
				empty_site_i = origin_i - 1;
				h_barrier = origin_i - 1;
			}
			
			// Sites
			naxos::NsIntVar* origin_site = model->get_position_qubits_constraint(origin_i, origin_j);
			pm->add(*origin_site == this->qubit_index);

			naxos::NsIntVar* ancilla_site = model->get_position_qubits_constraint(empty_site_i, empty_site_j);
			pm->add(*ancilla_site == this->qubit_index);

			// Barriers
			naxos::NsIntVar* middle_barrier = model->get_h_line_constraint(h_barrier);
			pm->add(*middle_barrier == 1);

			if (h_barrier > 0) {
				naxos::NsIntVar* bottom_barrier = model->get_h_line_constraint(h_barrier - 1);
				pm->add(*bottom_barrier == 0);
			}
			if (h_barrier < n - 2) {
				naxos::NsIntVar* top_barrier = model->get_h_line_constraint(h_barrier + 1);
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
			naxos::NsIntVar* origin_line = model->get_d_line_constraint(origin_j - origin_i);
			naxos::NsIntVar* empty_line = model->get_d_line_constraint(empty_site_j - empty_site_i);
			pm->add(*origin_line < *empty_line);
		}
	}
	
}

void Measurement::execute(CrossbarModel* model, int curr_cycle, bool with_animation, int speed) {
	if (curr_cycle >= -1 && curr_cycle <= 2) {
		// By default: never shuttle to ancilla qubit
	} else if (curr_cycle <= 8) {
		// Simulate QL wave
		model->evolve(this->get_involved_qubits());
	}
	
	// Qubit info
	/*Qubit* qubit = model->get_qubit(this->qubit_index);
	QubitPosition* pos = qubit->get_position();
	int m, n;
	std::tie(m, n) = model->get_dimensions();
	
	int origin_i = pos->get_i();
	int origin_j = pos->get_j();
	
	// Get ancilla
	Qubit* ancilla;
	int v_barrier;
	if (this->ancilla_direction == Measurement::DIR_ANCILLA_LEFT) {
		auto ancilla_index = *model->get_qubits(origin_i, origin_j - 1).begin();
		ancilla = model->get_qubit(ancilla_index);
		v_barrier = origin_j - 1;
	} else if (this->ancilla_direction == Measurement::DIR_ANCILLA_RIGHT) {
		auto ancilla_index = *model->get_qubits(origin_i, origin_j + 1).begin();
		ancilla = model->get_qubit(ancilla_index);
		v_barrier = origin_j;
	} else {
		throw std::runtime_error("Undefined measurement");
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
	if (this->site_direction == Measurement::DIR_SITE_UP) {
		empty_site_i = origin_i + 1;
		h_barrier = origin_i;
	} else if (this->site_direction == Measurement::DIR_SITE_DOWN) {
		empty_site_i = origin_i - 1;
		h_barrier = origin_i - 1;
	} else {
		throw std::runtime_error("Undefined measurement");
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
	if (with_animation) this->wait(waiting_seconds);*/
}
