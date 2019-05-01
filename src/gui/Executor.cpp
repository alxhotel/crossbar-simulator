#include "Executor.h"

Executor::Executor(CrossbarModel* model, bool active_animations,
	int speed, CodeEditor* editor, CrossbarGrid* grid,
	std::vector<std::vector<Operation*> > operations) {
	
	// Parameters
	this->model = model;
	this->active_animations = active_animations;
	this->speed = speed;
	
	// UI Components
	this->editor = editor;
	this->grid = grid;
	
	// Operations
	this->operations = operations;
}

Executor::~Executor() {
	
}

void Executor::doWork() {
	try {
		// Set editor to read only
		this->editor->setReadOnly(true);

		// First iteration to collect info about instructions and times
		Intervals::IntervalTree<int, Operation*> operations_interval = ConstraintChecker::get_intervals_by_operations(this->operations);
		
		// Set a reasonable max_cycle
		int max_cycle = ConstraintChecker::max_cycle(operations_interval);
		for (int curr_cycle = 0; curr_cycle < max_cycle; curr_cycle++) {
			// Get intersected intervals
			const auto &intervals = operations_interval.findIntervalsContainPoint(curr_cycle);
			
			// Highlight the current operation in editor
			//this->editor->setHighlightGray(operation->get_line_number());

			// Try to execute always
			for (const auto &interval : intervals) {
				if (interval.low <= curr_cycle && interval.high >= curr_cycle) {
					Operation* operation = interval.value;
					operation->execute(model, curr_cycle - interval.low);
				}
			}
			
			// 1. Find solution to dynamic constraints of starting and middle
			std::vector<Intervals::Interval<int, Operation*> > current_intervals;
			for (const auto &interval : intervals) {
				if (interval.high != curr_cycle) {
					current_intervals.push_back(interval);
				}
			}
			
			ConstraintChecker::solve_parameters(this->model, current_intervals, curr_cycle);
			
			int m, n;
			std::tie(m, n) = model->get_dimensions();
			
			// Apply the solution
			for (int k = 0; k < m - 1; k++) {
				if (this->model->get_h_line_constraint(k)->value() == 0) {
					this->model->raise_h_line(k);
				} else {
					this->model->lower_h_line(k);
				}
				
				if (this->model->get_v_line_constraint(k)->value() == 0) {
					this->model->raise_v_line(k);
				} else {
					this->model->lower_v_line(k);
				}
			}
			for (int k = -1 * (m - 1); k <= (m - 1); k++) {
				if (this->model->get_d_line(k) != this->model->get_d_line_constraint(k)->value()) {
					this->model->set_d_line(k, this->model->get_d_line_constraint(k)->value());
				}
			}
			if ((this->model->get_active_wave() == 0 && this->model->get_wave_constraint()->value() != 0)
				|| (this->model->get_active_wave() != 0 && this->model->get_wave_constraint()->value() == 0)) { 
				this->model->toggle_wave(this->model->get_wave_column_constraint()->value() == 0);
			}
			
			emit cycle_done(curr_cycle);
			
			// Simulate each cycle by waiting
			if (this->active_animations) QThread::msleep(this->get_waiting_seconds(this->speed) * 1000);
		}

		// Reset editor mode
		this->editor->setReadOnly(false);
		//this->editor->clearManualSelections();

		emit finished_ok();
	} catch (std::exception& ex) {
		std::cout << ex.what() << std::endl << std::flush;
		
		emit finished_err(ex.what());
	}
	
	emit finished();
}

double Executor::get_waiting_seconds(int speed) {
	return 1 / (double) speed;
}
