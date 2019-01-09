#include <math.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include "CrossbarModel.h"

CrossbarModel::CrossbarModel(int num_qubits) {	
	// Set the subscribers
	this->subscribers = {};
	
	// Init crossbar by using aux method
	this->resize(num_qubits);
}

CrossbarModel::~CrossbarModel() {
	delete this->pm;
}

CrossbarModel* CrossbarModel::clone() {
	CrossbarModel* cloned_model = new CrossbarModel(this->num_qubits);
	
	for (auto const &barrier : this->h_lines) {
		cloned_model->h_lines[barrier.first]->set_value(barrier.second->get_value());
	}
	for (auto const &barrier : this->v_lines) {
		cloned_model->v_lines[barrier.first]->set_value(barrier.second->get_value());
	}
	for (auto const &ql_line : this->d_lines) {
		cloned_model->d_lines[ql_line.first]->set_value(ql_line.second->get_value());
	}
	
	//std::map<int, std::map<int, int> cloned_model->positions_qubits(this->positions_qubits);
	//std::map<int, Qubit*> cloned_model->qubits(this->qubits);
	
	for (auto const &qubit : this->qubits) {
		QubitPosition* pos = qubit.second->get_position();
		cloned_model->move_qubit(qubit.first, pos->get_i(), pos->get_j());
	}
	
	// TODO: ancillary qubits
	//std::vector<Subscriber*> cloned_model->subscribers(this->subscribers);
	
	return cloned_model;
}

void CrossbarModel::init_constraints() {
	// TODO: fix this commented line
	//if (this->pm != NULL) delete this->pm;
	this->pm = new naxos::NsProblemManager();
}

void CrossbarModel::add_static_constraints() {
	// Init constraints
	this->init_constraints();
	
	// TODO: free memory
	for (int i = 0; i <= m - 2; i++) {
		this->h_lines_constraint[i] = new naxos::NsIntVar(*this->pm, 0, 1);
	}
	
	for (int j = 0; j <= n - 2; j++) {
		this->v_lines_constraint[j] = new naxos::NsIntVar(*this->pm, 0, 1);
	}
	
	for (int k = -1 * (n - 1); k <= m; k++) {
		this->d_lines_constraint[k] = new naxos::NsIntVar(*this->pm, 0, CrossbarModel::MAX_QL_VOLTAGE);
	}
	
	for (int i = 0; i < m; ++i) {
		this->position_qubits_constraint[i] = {};
		for (int j = 0; j < n; ++j) {
			this->position_qubits_constraint[i][j] = new naxos::NsIntVar(*this->pm, 0, 2);
			this->pm->add(
				*this->position_qubits_constraint[i][j] == this->positions_qubits[i][j].size()
			);
		}
	}
}

naxos::NsProblemManager* CrossbarModel::get_problem_manager() {
	return this->pm;
}

naxos::NsIntVar* CrossbarModel::get_h_line_constraint(int i) {
	return this->h_lines_constraint[i];
}

naxos::NsIntVar* CrossbarModel::get_v_line_constraint(int i) {
	return this->v_lines_constraint[i];
}

naxos::NsIntVar* CrossbarModel::get_d_line_constraint(int i) {
	return this->d_lines_constraint[i];
}

std::map<int, std::map<int, naxos::NsIntVar*> > CrossbarModel::get_position_qubits_constraint() {
	return this->position_qubits_constraint;
}

naxos::NsIntVar* CrossbarModel::get_position_qubits_constraint(int i, int j) {
	return this->position_qubits_constraint[i][j];
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

bool CrossbarModel::is_h_barrier_up(int i) {
	if (i < 0 || unsigned(i) > this->h_lines.size() - 1) {
		return true;
	}
	return this->h_lines[i]->is_up();
}
	
bool CrossbarModel::is_v_barrier_up(int i) {
	if (i < 0 || unsigned(i) > this->v_lines.size() - 1) {
		return true;
	}
	return this->v_lines[i]->is_up();
}

bool CrossbarModel::is_h_barrier_down(int i) {
	if (i < 0 || unsigned(i) > this->h_lines.size() - 1) {
		return false;
	}
	return this->h_lines[i]->is_down();
}

bool CrossbarModel::is_v_barrier_down(int i) {
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
 * Notify a change in control lines to all subscribers
 */
void CrossbarModel::notify_all() {
	for (Subscriber* sub : this->subscribers) {
		sub->notified();
	}
}

/**
 * Notify a change of size to all subscribers
 */
void CrossbarModel::notify_resize_all() {
	for (Subscriber* sub : this->subscribers) {
		sub->notified_resize();
	}
}

void CrossbarModel::toggle_wave(bool is_even_column) {
	if (this->active_wave == 0) {
		this->active_wave = (is_even_column) ? 2 : 1;
	} else {
		this->active_wave = 0;
	}
	this->notify_all();
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
		if ((this->is_h_barrier_down(i) && this->is_h_barrier_down(i + 1))
			|| (this->is_h_barrier_down(i - 1) && this->is_h_barrier_down(i - 2))) {
			throw std::runtime_error("Undecidable configuration in (" + std::to_string(i) + ", " + std::to_string(j) + ")");
		}
		if ((this->is_v_barrier_down(j) && this->is_v_barrier_down(j + 1))
			|| (this->is_v_barrier_down(j - 1) && this->is_v_barrier_down(j - 2))) {
			throw std::runtime_error("Undecidable configuration in (" + std::to_string(i) + ", " + std::to_string(j) + ")");
		}
		// More than 2 open barriers
		int count_barriers_down = int(this->is_h_barrier_down(i)) + int(this->is_h_barrier_down(i - 1)) 
			+ int(this->is_v_barrier_down(j)) + int(this->is_v_barrier_down(j - 1));
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
			if (this->is_h_barrier_down(i)) {
				this->move_qubit(q_id, i + 1, j);
			}
			// Shuttle to the left
			if (this->is_v_barrier_down(j - 1)) {
				this->move_qubit(q_id, i, j - 1);
			}
		}
		
		if (d_line_bottom_val > d_line_middle_val) {
			// Shuttle to the bottom
			if (this->is_h_barrier_down(i - 1)) {
				this->move_qubit(q_id, i - 1, j);
			}
			// Shuttle to the right
			if (this->is_v_barrier_down(j)) {
				this->move_qubit(q_id, i, j + 1);
			}
		}
	}
	
	this->notify_all();
}

void CrossbarModel::move_qubit(int q_id, int i_dest, int j_dest) {
	// TODO: Validate params
	QubitPosition* pos = this->qubits[q_id]->get_position();
	this->positions_qubits[pos->get_i()][pos->get_j()].erase(q_id);
	this->positions_qubits[i_dest][j_dest].insert(q_id);
	pos->set_i(i_dest);
	pos->set_j(j_dest);
	this->notify_all();
}

/**
 * Apply a difference in QL voltage, where VL(origin) < VL (destination)
 */
void CrossbarModel::apply_ql(int origin_i, int origin_j, int dest_i, int dest_j, int flag) {
	// Depends on the shuttling
	std::map<int, double> changed_d_lines = {};
	if (origin_j == dest_j) {
		// Vertical
		int top_i, bottom_i;
		if (origin_i < dest_i) {
			top_i = dest_i;
			bottom_i = origin_i;
		} else {
			top_i = origin_i;
			bottom_i = dest_i;
		}
		
		double default_value = 0;
		for (int j = 0; j < this->n; j++) {
			/*if (!this->positions_qubits[top_i][j].empty()
					&& !this->positions_qubits[bottom_i][j].empty()) {
				throw std::runtime_error("There are two qubits in the same column " + std::to_string(j) + " while shuttling");
			}*/
			
			// Use default value
			this->d_lines[j - top_i ]->set_value(default_value);
			
			// Inverse strategy for the shuttling case
			int shuttling_flag = 1;
			if (j == origin_j) shuttling_flag = flag;
			
			if (!this->positions_qubits[top_i][j].empty()) {
				// Right occupied
				this->d_lines[j - bottom_i]->set_value(this->d_lines[j - top_i]->get_value() - (1 * shuttling_flag));
			} else if (!this->positions_qubits[bottom_i][j].empty()) {
				// Left occupied
				this->d_lines[j - bottom_i]->set_value(this->d_lines[j - top_i]->get_value() + (1 * shuttling_flag));
			} else {
				// Both empty: do nothing
				this->d_lines[j - bottom_i]->set_value(this->d_lines[j - top_i]->get_value());
			}
			
			// Update default value
			default_value = this->d_lines[j - bottom_i]->get_value();
			
			changed_d_lines[j - top_i] = this->d_lines[j - top_i]->get_value();
			changed_d_lines[j - bottom_i] = this->d_lines[j - bottom_i]->get_value();
		}
	} else {
		// Horizontal
		int left_j, right_j;
		if (origin_j < dest_j) {
			left_j = origin_j;
			right_j = dest_j;
		} else {
			left_j = dest_j;
			right_j = origin_j;
		}
		
		int default_value = 0;
		for (int i = 0; i < this->m; i++) {
			/*if (!this->positions_qubits[i][left_j].empty()
					&& !this->positions_qubits[i][right_j].empty()) {
				throw std::runtime_error("There are two qubits in the same row " + std::to_string(i) + " while shuttling");
			}*/
			
			// Use default value
			this->d_lines[right_j - i]->set_value(default_value);
			
			// Inverse strategy for the shuttling case
			int shuttling_flag = 1;
			if (i == origin_i) shuttling_flag = -1;
			
			if (!this->positions_qubits[i][right_j].empty()) {
				// Right occupied
				this->d_lines[left_j - i]->set_value(this->d_lines[right_j - i]->get_value() - (1 * shuttling_flag));
			} else if (!this->positions_qubits[i][left_j].empty()) {
				// Left occupied
				this->d_lines[left_j - i]->set_value(this->d_lines[right_j - i]->get_value() + (1 * shuttling_flag));
			} else {
				// Both empty: do nothing
				this->d_lines[left_j - i]->set_value(this->d_lines[right_j - i]->get_value());
			}
			
			// Update default value
			default_value = this->d_lines[left_j - i]->get_value();
			
			changed_d_lines[right_j - i] = this->d_lines[right_j - i]->get_value();
			changed_d_lines[left_j - i] = this->d_lines[left_j - i]->get_value();
		}
	}
	
	// Normalize voltages (all positive) of the changed QL lines
	double min_value = changed_d_lines.begin()->second;
	std::map<int, double>::iterator it;
	for (it = changed_d_lines.begin(); it != changed_d_lines.end(); it++) {
		if (it->second < min_value) {
			min_value = it->second;
		}
	}
	for (it = changed_d_lines.begin(); it != changed_d_lines.end(); it++) {
		this->d_lines[it->first]->set_value(this->d_lines[it->first]->get_value() + (-1 * min_value));
	}
}

void CrossbarModel::apply_diff_ql(int origin_i, int origin_j, int dest_i, int dest_j) {
	this->apply_ql(origin_i, origin_j, dest_i, dest_j, -1);
}

void CrossbarModel::apply_eq_ql(int origin_i, int origin_j, int dest_i, int dest_j) {
	this->apply_ql(origin_i, origin_j, dest_i, dest_j, 0);
}

int CrossbarModel::get_active_wave() {
	return this->active_wave;
}

Qubit* CrossbarModel::get_qubit(int q_id) {
	if (this->qubits.find(q_id) == this->qubits.end()) return NULL;
	else return this->qubits[q_id];
}

QubitPosition* CrossbarModel::get_position(int q_id) {
	if (this->qubits.find(q_id) == this->qubits.end()) return NULL;
	else return this->qubits[q_id]->get_position();
}

std::set<int> CrossbarModel::get_qubits(int i, int j) {
	// Validate params
	if (this->positions_qubits.find(i) != this->positions_qubits.end()
			&& this->positions_qubits[i].find(j) != this->positions_qubits[i].end()) {
		return this->positions_qubits[i][j];
	} else {
		std::cout << i << " " << j << std::endl << std::flush;
		throw std::runtime_error("Invalid coordinates");
	}
}

std::map<int, Qubit*> CrossbarModel::iter_qubits_positions() {
	return this->qubits;
}

/**
 * Reset the crossbar to its original state (same number of qubits and size)
 */
void CrossbarModel::reset() {
	// Reset wave line
	this->active_wave = 0;
	
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

/**
 * Resets the crossbar to a new size of qubits (and maybe size)
 * @param num_qubits
 */
void CrossbarModel::resize(int num_qubits) {
	// Create a square layout for the number of qubits
	int dim = (num_qubits % 2 == 0) ? ceil(sqrt(num_qubits * 2)) : ceil(sqrt(num_qubits * 2 - 1));
	this->m = dim;
	this->n = dim;
	this->num_qubits = num_qubits;
	
	// Wave
	this->active_wave = 0;
	
	// Create horizontal, vertical & diagonal control lines;
	this->h_lines.clear();
	for (int i = 0; i <= this->m - 2; i++) this->h_lines[i] = new BarrierLine(0);
	
	this->v_lines.clear();
	for (int j = 0; j <= this->n - 2; j++) this->v_lines[j] = new BarrierLine(0);
	
	this->d_lines.clear();
	for (int k = -1 * (this->n - 1); k <= this->m; k++) this->d_lines[k] = new QubitLine(1.0 + abs(k) % 2);
	
	// Create qubits & positions
	this->positions_qubits.clear();
	this->qubits.clear();
	int q_id = 0;
	for (int i = 0; i < this->m; ++i) {
		this->positions_qubits[i] = {};
		for (int j = 0; j < this->n; ++j) {
			if ((i + j) % 2 == 0) {
				// Fill until we finish with all qubits 
				if (q_id < this->num_qubits) {
					this->qubits[q_id] = new Qubit(
						(j % 2 == 0) ? new QubitState(0, 1) : new QubitState(1, 0),
						new QubitPosition(i, j),
						(i % 2 != 0)
					);
					this->positions_qubits[i][j] = {q_id};
					q_id++;
				}
			} else {
				this->positions_qubits[i][j] = {};
			}
		}
	}
		
	// Init constraints
	this->init_constraints();
	
	this->notify_resize_all();
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
