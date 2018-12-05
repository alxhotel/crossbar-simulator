#include <string>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include "CrossbarModel.h"

CrossbarModel::CrossbarModel(int m, int n) {
	this->m = m;
	this->n = n;
	
	// Create horizontal, vertical & diagonal control lines;
	for (int i = 0; i <= m - 2; i++) this->h_lines[i] = new BarrierLine(0);
	for (int j = 0; j <= n - 2; j++) this->v_lines[j] = new BarrierLine(0);
	for (int k = -1 * (n - 1); k <= m; k++) this->d_lines[k] = new QubitLine(1.0 + abs(k) % 2);

	// Create qubits & positions
	int q_id = 0;
	for (int i = 0; i < m; ++i) {
		this->positions_qubits[i] = {};
		for (int j = 0; j < n; ++j) {
			if ((i + j) % 2 == 0) {
				this->qubits[q_id] = new Qubit(new QubitState(1, 0), new QubitPosition(i, j));
				this->positions_qubits[i][j] = {q_id};
				q_id++;
			} else {
				this->positions_qubits[i][j] = {};
			}
		}
	}
	
	// Auxiliary structures
	this->ancillary_qubits = {};
	
	// Set the subscribers
	this->subscribers = {}; 
}

/**
 * Get the dimensions of the crossbar
 * @return 
 */
std::tuple<int, int> CrossbarModel::get_dimensions() {
	return std::make_tuple(this->m, this->n);
}

/**
 * Get the control line dimensions
 * @return 
 */
std::tuple<int, int, int> CrossbarModel::get_control_line_dimensions() {
	return std::make_tuple(this->h_lines.size(), this->v_lines.size(), this->d_lines.size());
}

bool CrossbarModel::h_barrier_up(int i) {
	if (i < 0 || unsigned(i) > this->h_lines.size() - 1) {
		return true;
	}
	return this->h_lines[i]->is_up();
}
	
bool CrossbarModel::v_barrier_up(int i) {
	if (i < 0 || unsigned(i) > this->v_lines.size() - 1) {
		return true;
	}
	return this->v_lines[i]->is_up();
}

bool CrossbarModel::h_barrier_down(int i) {
	if (i < 0 || unsigned(i) > this->h_lines.size() - 1) {
		return false;
	}
	return this->h_lines[i]->is_down();
}

bool CrossbarModel::v_barrier_down(int i) {
	if (i < 0 || unsigned(i) > this->v_lines.size() - 1) {
		return false;
	}
	return this->v_lines[i]->is_down();
}

float CrossbarModel::d_line_value(int i) {
	return this->d_lines[i]->get_value();
}

/**
 * Subscribe to the notifications
 * @param subscriber
 */
void CrossbarModel::subscribe(Subscriber* subscriber) {
	this->subscribers.emplace_back(subscriber);
	subscriber->notified();
}

/**
 * Notify a change to all subscribers
 */
void CrossbarModel::notify_all() {
	for (Subscriber* sub : this->subscribers) {
		sub->notified();
	}
}

void CrossbarModel::toggle_h_line(int i) {
	this->h_lines[i]->toggle();
	std::cout << "RL[" << std::to_string(i) << "] new value = "
			<< std::to_string(this->h_lines[i]->get_value()).substr(0, 3) << std::endl << std::flush;
	this->notify_all();
}

void CrossbarModel::toggle_v_line(int i) {
	this->v_lines[i]->toggle();
	std::cout << "CL[" << std::to_string(i) << "] new value = "
			<< std::to_string(this->v_lines[i]->get_value()).substr(0,3) << std::endl << std::flush;
	this->notify_all();
}

void CrossbarModel::change_d_line(int i, int (*func)(int)) {
	int new_value = func(this->d_lines[i]->get_value());
	std::cout << "QL[" << std::to_string(i) <<  "] new value: " << std::to_string(new_value) << std::endl << std::flush;
	this->d_lines[i]->set_value(new_value);
	this->notify_all();
}

/**
 * Check if the configuration is valid
 */
void CrossbarModel::check_valid_configuration() {
	for (auto const &entry : this->iter_qubits_positions()) {
		QubitPosition* pos = entry.second->get_position();
		int i = pos->get_i();
		int j = pos->get_j();
		
		// The target site has two or more open barriers
		// | q : x : x |
		if ((this->h_barrier_down(i) && this->h_barrier_down(i + 1))
			|| (this->h_barrier_down(i - 1) && this->h_barrier_down(i - 2))) {
			throw std::runtime_error("Undecidable configuration in (" + std::to_string(i) + ", " + std::to_string(j) + ")");
		}
		if ((this->v_barrier_down(j) && this->v_barrier_down(j + 1))
			|| (this->v_barrier_down(j - 1) && this->v_barrier_down(j - 2))) {
			throw std::runtime_error("Undecidable configuration in (" + std::to_string(i) + ", " + std::to_string(j) + ")");
		}
		// More than 2 open barriers
		int count_barriers_down = int(this->h_barrier_down(i)) + int(this->h_barrier_down(i - 1)) 
			+ int(this->v_barrier_down(j)) + int(this->v_barrier_down(j - 1));
		if (count_barriers_down > 1) {
			throw std::runtime_error("Undecidable configuration in (" + std::to_string(i) + ", " + std::to_string(j) + ")");
		}
	}
	
}

void CrossbarModel::evolve() {
	// First, check any conflicts in the configuration
	this->check_valid_configuration();
	
	for (auto const &entry : this->iter_qubits_positions()) {
		int q_id = entry.first;
		QubitPosition* pos = entry.second->get_position();
		
		int i = pos->get_i();
		int j = pos->get_j();
		double d_line_top_val = this->d_lines[std::max((this->m - 1) * -1, j - i - 1)]->get_value();
		double d_line_middle_val = this->d_lines[j - i]->get_value();
		double d_line_bottom_val = this->d_lines[std::min(j - i + 1, (this->m - 1))]->get_value();

		if (d_line_top_val > d_line_middle_val) {
			// Shuttle to the top
			if (this->h_barrier_down(i)) {
				this->move_qubit(q_id, i + 1, j);
			}
			// Shuttle to the left
			if (this->v_barrier_down(j - 1)) {
				this->move_qubit(q_id, i, j - 1);
			}
		}
		
		if (d_line_bottom_val > d_line_middle_val) {
			// Shuttle to the bottom
			if (this->h_barrier_down(i - 1)) {
				this->move_qubit(q_id, i - 1, j);
			}
			// Shuttle to the right
			if (this->v_barrier_down(j)) {
				this->move_qubit(q_id, i, j + 1);
			}
		}
	}
	
	this->notify_all();
}

void CrossbarModel::move_qubit(int q_id, int i_dest, int j_dest) {
	QubitPosition* pos = this->qubits[q_id]->get_position();
	this->qubits[q_id]->set_position(new QubitPosition(i_dest, j_dest));
	this->positions_qubits[pos->get_i()][pos->get_j()].erase(q_id);
	this->positions_qubits[i_dest][j_dest].insert(q_id);
}

QubitPosition* CrossbarModel::get_position(int q_id) {
	if (this->qubits.find(q_id) == this->qubits.end()) return NULL;
	else return this->qubits[q_id]->get_position();
}

std::set<int> CrossbarModel::get_qubits(int i, int j) {
	// Validate params
	if (i < 0 || i >= this->m || j < 0 || j >= this->n) return {1};
	
	//if (this->positions_qubits.find(i) != this->positions_qubits.end()
	//		&& this->positions_qubits[i].find(j) != this->positions_qubits.end()) {
		return this->positions_qubits[i][j];
	//} else {
	//	return NULL;
	//}
}

std::map<int, Qubit*> CrossbarModel::iter_qubits_positions() {
	return this->qubits;
}

void CrossbarModel::reset() {
	// Reset control lines
	for (int i = 0; i <= m - 2; i++) this->h_lines[i]->set_value(0);
	for (int j = 0; j <= n - 2; j++) this->v_lines[j]->set_value(0);
	for (int k = -1 * (n - 1); k <= m; k++) this->d_lines[k]->set_value(1.0 + abs(k) % 2);

	// Reset qubits & positions
	int q_id = 0;
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			if ((i + j) % 2 == 0) {
				this->qubits[q_id]->get_state()->reset();
				this->qubits[q_id]->get_position()->set_i(i);
				this->qubits[q_id]->get_position()->set_j(j);
				this->positions_qubits[i][j].clear();
				this->positions_qubits[i][j].insert(q_id);
				q_id++;
			} else {
				this->positions_qubits[i][j].clear();
			}
		}
	}
	
	// TODO: ancillary qubits
	
	this->notify_all();
}

bool CrossbarModel::is_edge(int i) {
	return this->is_top_edge(i) || this->is_bottom_edge(i);
}

bool CrossbarModel::is_top_edge(int i) {
	return (i == this->m - 1);
}

bool CrossbarModel::is_bottom_edge(int i) {
	return (i == 0);
}
