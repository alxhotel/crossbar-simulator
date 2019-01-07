#ifndef OPERATION_H
#define OPERATION_H

#include <thread>
#include <QThread>
#include <iostream>
#include <naxos.h>
#include "crossbar/CrossbarModel.h"

class Operation {
public:
	// Latency in ns
	static int LATENCY_NS;
	
	virtual void add_static_constraints(CrossbarModel* model) = 0;
	
	virtual void add_dynamic_constraints(CrossbarModel* model) = 0;
	
	virtual void execute(CrossbarModel* model, bool with_animations = false, int speed = 1) = 0;
	
	int getLineNumber() {
		return this->line_number;
	}
	
	double get_waiting_seconds(int speed) {
		return 1 / (double) speed;
	}
	
protected:
	int line_number;

	void wait(double seconds) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(seconds));
		QThread::msleep(seconds * 1000);
	}
	
};

#endif /* OPERATION_H */

