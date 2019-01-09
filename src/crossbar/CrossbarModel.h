#ifndef CROSSBAR_SIMULATOR_CROSSBARMODEL_H
#define CROSSBAR_SIMULATOR_CROSSBARMODEL_H

#include <map>
#include <set>
#include <vector>
#include <tuple>
#include <naxos.h>
#include "control-lines/QubitLine.h"
#include "control-lines/BarrierLine.h"
#include "Qubit.h"
#include "QubitState.h"
#include "QubitPosition.h"
#include "crossbar/Subscriber.h"

class CrossbarModel {
public:
	static const long MAX_QL_VOLTAGE = 8000;
	
	CrossbarModel(int num_qubits);
	
	~CrossbarModel();
	
	CrossbarModel* clone();
	
	// CONSTRAINTS
	void init_constraints();
	void add_static_constraints();
	naxos::NsProblemManager* get_problem_manager();
	naxos::NsIntVar* get_h_line_constraint(int i);
	naxos::NsIntVar* get_v_line_constraint(int i);
	naxos::NsIntVar* get_d_line_constraint(int i);
	std::map<int, std::map<int, naxos::NsIntVar*> > get_position_qubits_constraint();
	naxos::NsIntVar* get_position_qubits_constraint(int i, int j);
	
	std::tuple<int, int> get_dimensions();
	
	std::tuple<int, int, int> get_control_line_dimensions();
	
	// Control lines
	bool is_h_barrier_up(int i);
	bool is_h_barrier_down(int i);
	bool is_v_barrier_up(int i);
	bool is_v_barrier_down(int i);
	float d_line_value(int i);
	void toggle_h_line(int i);
	void toggle_v_line(int i);
	void change_d_line(int i, int (*func)(int));
	
	// Operations
	void check_valid_configuration();
	void evolve();
	void move_qubit(int q_id, int i_dest, int j_dest);
	void apply_ql(int origin_i, int origin_j, int dest_i, int dest_j, int flag);
	void apply_diff_ql(int origin_i, int origin_j, int dest_i, int dest_j);
	void apply_eq_ql(int origin_i, int origin_j, int dest_i, int dest_j);
	void toggle_wave(bool is_even_column);
	
	int get_active_wave();
	
	void reset();
	void resize(int num_qubits);
	
	Qubit* get_qubit(int q_id);
	QubitPosition* get_position(int q_id);
	std::set<int> get_qubits(int i, int j);

	// TODO: fix
	std::map<int, Qubit*> iter_qubits_positions();
	
	void subscribe(Subscriber* subscriber);
	void notify_all();
	void notify_resize_all();
	
private:
	// Dimensions
	int m;
	int n;
	int num_qubits;
	
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
	naxos::NsProblemManager* pm;
	std::map<int, naxos::NsIntVar*> h_lines_constraint;
	std::map<int, naxos::NsIntVar*> v_lines_constraint;
	std::map<int, naxos::NsIntVar*> d_lines_constraint;
	std::map<int, std::map<int, naxos::NsIntVar*> > position_qubits_constraint;
	
	bool is_edge(int i);
	bool is_top_edge(int i);
	bool is_bottom_edge(int i);
};

#endif //CROSSBAR_SIMULATOR_CROSSBARMODEL_H
