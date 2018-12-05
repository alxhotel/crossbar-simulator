#ifndef CROSSBAR_SIMULATOR_CROSSBARMODEL_H
#define CROSSBAR_SIMULATOR_CROSSBARMODEL_H

#include <map>
#include <set>
#include <vector>
#include <tuple>
#include "control-lines/QubitLine.h"
#include "control-lines/BarrierLine.h"
#include "Qubit.h"
#include "QubitState.h"
#include "QubitPosition.h"
#include "crossbar/Subscriber.h"

class CrossbarModel {
public:
	CrossbarModel(int m, int n);

	std::tuple<int, int> get_dimensions();
	
	std::tuple<int, int, int> get_control_line_dimensions();
	
	bool h_barrier_up(int i);
	
	bool v_barrier_up(int i);
	
	bool h_barrier_down(int i);
	
	bool v_barrier_down(int i);
	
	float d_line_value(int i);
	
	void subscribe(Subscriber* subscriber);
	
	void notify_all();
		
	void toggle_h_line(int i);
	
	void toggle_v_line(int i);
	
	void change_d_line(int i, int (*func)(int));
	
	void check_valid_configuration();
	
	void evolve();

	void move_qubit(int q_id, int i_dest, int j_dest);
	
	QubitPosition* get_position(int q_id);
	
	std::set<int> get_qubits(int i, int j);

	// TODO: fix
	std::map<int, Qubit*> iter_qubits_positions();
	
	void reset();
	
private:
	int m;
	int n;

	// Control lines
	std::map<int, BarrierLine*> h_lines;
	std::map<int, BarrierLine*> v_lines;
	std::map<int, QubitLine*> d_lines;

	// Qubit positions
	std::map<int, Qubit*> qubits;
	std::map<int, std::map<int, std::set<int>>> positions_qubits;
	std::vector<Qubit*> ancillary_qubits;
	
	// Notification system
	std::vector<Subscriber*> subscribers;
	
	bool is_edge(int i);
	
	bool is_top_edge(int i);
	
	bool is_bottom_edge(int i);
};

#endif //CROSSBAR_SIMULATOR_CROSSBARMODEL_H
