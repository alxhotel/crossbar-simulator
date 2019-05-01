#include "BarrierLine.h"

BarrierLine::BarrierLine(int mode, double state) {
	this->mode = mode;
	this->state = state;
}

BarrierLine::BarrierLine(double state) {
	this->mode = MODE::BARRIER;
	this->state = state;
}

int BarrierLine::get_mode() const {
	return this->mode;
}

void BarrierLine::set_mode(int mode) {
	this->mode = mode;
}

double BarrierLine::get_state() const {
	return this->state;
}

void BarrierLine::set_state(double state) {
	this->state = state;
}

bool BarrierLine::is_up() const {
	return (this->get_state() == STATE::RAISED);
}

bool BarrierLine::is_down() const {
	return (this->get_state() == STATE::LOWERED);
}

bool BarrierLine::is_open() const {
	return this->is_down();
}

bool BarrierLine::is_closed() const {
	return this->is_up();
}

void BarrierLine::open() {
	this->set_state(STATE::LOWERED);
}

void BarrierLine::close() {
	this->set_state(STATE::RAISED);
}

void BarrierLine::toggle() {
	if (this->get_state() == STATE::RAISED) this->set_state(STATE::LOWERED);
	else this->set_state(STATE::RAISED);
}
