#ifndef CROSSBAR_SIMULATOR_CROSSBARMODEL_H
#define CROSSBAR_SIMULATOR_CROSSBARMODEL_H

#include <map>
#include <set>
#include <vector>
#include <tuple>
#include <math.h>
#include <algorithm>
#include <naxos.h>

#include "control-lines/QubitLine.h"
#include "control-lines/BarrierLine.h"
#include "Qubit.h"
#include "QubitState.h"
#include "QubitPosition.h"
#include "crossbar/Subscriber.h"

class CrossbarModel {
public:
	static const long MAX_QL_VOLTAGE = 100;
	
	CrossbarModel(int m, int n, int data_qubits, int ancilla_qubits);
	CrossbarModel(int size, int data_qubits, int ancilla_qubits);
	
	~CrossbarModel();
	
	CrossbarModel* clone();
	
	// CONSTRAINTS
	void init_constraints();
	void add_constraints();
	naxos::NsProblemManager* get_problem_manager();
	naxos::NsIntVar* get_h_line_constraint(int i);
	naxos::NsIntVar* get_v_line_constraint(int i);
	naxos::NsIntVar* get_d_line_constraint(int i);
	std::map<int, std::map<int, naxos::NsIntVar*> > get_position_qubits_constraint();
	naxos::NsIntVar* get_position_qubits_constraint(int i, int j);
	naxos::NsIntVar* get_wave_constraint();
	naxos::NsIntVar* get_wave_column_constraint();
	void save_constraint_solution();
	void restore_constraint_solution();
	
	int get_data_qubits();
	int get_ancilla_qubits();
	std::tuple<int, int> get_dimensions();
	std::tuple<int, int, int> get_control_line_dimensions();
	
	// Control lines
	bool is_h_barrier_up(int i);
	bool is_h_barrier_down(int i);
	bool is_v_barrier_up(int i);
	bool is_v_barrier_down(int i);
	float get_d_line(int i);
	void toggle_h_line(int i);
	void toggle_v_line(int i);
	void lower_h_line(int i);
	void raise_h_line(int i);
	void lower_v_line(int i);
	void raise_v_line(int i);
	void change_d_line(int i, int (*func)(int));
	void set_d_line(int i, int new_value);
	
	// Operations
	void check_valid_configuration();
	void evolve();
	void evolve(std::vector<int> involved_qubits);
	void move_qubit(int q_id, int i_dest, int j_dest);
	void apply_ql(int origin_i, int origin_j, int dest_i, int dest_j, int flag);
	void apply_diff_ql(int origin_i, int origin_j, int dest_i, int dest_j);
	void apply_eq_ql(int origin_i, int origin_j, int dest_i, int dest_j);
	void toggle_wave(bool is_even_column);
	
	int get_active_wave();
	
	void reset();
	void resize(int m, int n, int data_qubits, int ancilla_qubits);
	
	void add_qubit(int q_id, Qubit* qubit);
	void set_positions_qubits(int i, std::map<int, std::set<int> > q_map);
	void set_positions_qubits(int i, int j, std::set<int> q_set);
	
	Qubit* get_qubit(int q_id);
	QubitPosition* get_position(int q_id);
	std::set<int> get_qubits(int i, int j);
	std::set<int> get_qubits(int site);
	int get_num_qubits();

	// Configurations
	void idle_configuration();
	void inline_configuration();
	
	// TODO: fix
	std::map<int, Qubit*> iter_qubits_positions();
	
	void subscribe(Subscriber* subscriber);
	void unsubscribeAll();
	void notify_all();
	void notify_resize_all();
	
private:
	// Original
	CrossbarModel* original_model = NULL;
	
	// Parameters
	int m;
	int n;
	int data_qubits;
	int ancilla_qubits;
	
	// Wave (0: inactive, 1: odd, 2: even)
	int active_wave;

	// Control lines
	std::map<int, BarrierLine*> h_lines;
	std::map<int, BarrierLine*> v_lines;
	std::map<int, QubitLine*> d_lines;

	// Qubit positions
	std::map<int, Qubit*> qubits;
	std::map<int, std::map<int, std::set<int>>> positions_qubits;
	
	// Notification system
	std::vector<Subscriber*> subscribers;
	
	// Variables for the constraints checker
	naxos::NsProblemManager* pm = NULL;
	std::map<int, naxos::NsIntVar*> h_lines_constraint;
	std::map<int, naxos::NsIntVar*> v_lines_constraint;
	std::map<int, naxos::NsIntVar*> d_lines_constraint;
	std::map<int, std::map<int, naxos::NsIntVar*> > position_qubits_constraint;
	naxos::NsIntVar* wave_constraint = NULL;
	naxos::NsIntVar* wave_column_constraint = NULL;
	
	// Store the latest solution
	std::map<int, naxos::NsIntVar*> backup_h_lines_constraint;
	std::map<int, naxos::NsIntVar*> backup_v_lines_constraint;
	std::map<int, naxos::NsIntVar*> backup_d_lines_constraint;
	std::map<int, std::map<int, naxos::NsIntVar*> > backup_position_qubits_constraint;
	naxos::NsIntVar* backup_wave_constraint = NULL;
	naxos::NsIntVar* backup_wave_column_constraint = NULL;
	
	bool is_edge(int i);
	bool is_top_edge(int i);
	bool is_bottom_edge(int i);
	
	bool contains(std::vector<int> list, int element);
};

#endif //CROSSBAR_SIMULATOR_CROSSBARMODEL_H
