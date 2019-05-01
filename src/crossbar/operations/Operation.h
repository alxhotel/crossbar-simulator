#ifndef OPERATION_H
#define OPERATION_H

#include <thread>
#include <QThread>
#include <iostream>
#include <naxos.h>

#include "crossbar/CrossbarModel.h"

class Operation {
public:	
	Operation(int latency, int duration) {
		this->latency_ns = latency;
		this->duration_ns = duration;
	}
	
	virtual void check_static_constraints(CrossbarModel* model) = 0;
	
	virtual void add_dynamic_constraints(CrossbarModel* model, int curr_cycle) = 0;
	
	virtual void execute(CrossbarModel* model, int curr_cycle, bool with_animations = false, int speed = 1) = 0;
	
	virtual std::vector<int> get_involved_qubits() = 0;
	
	int get_line_number() {
		return this->line_number;
	}
	
	virtual int get_cycle_duration(int cycle_time) {
		return (int) (this->duration_ns / cycle_time);
	}
	
	friend std::ostream& operator<<(std::ostream& strm, const Operation& gate) {
		return strm << "Operation at line" << std::to_string(gate.line_number);
	}
	
protected:
	int latency_ns;
	int duration_ns;
	int line_number;

	void wait(double seconds) {
		QThread::msleep(seconds * 1000);
	}

	double get_waiting_seconds(int speed) {
		return 1 / (double) speed;
	}
	
	bool is_cycle(int curr_cycle, int cycle) {
		return (curr_cycle == cycle) || curr_cycle == -1;
	}
	
};

#endif /* OPERATION_H */
