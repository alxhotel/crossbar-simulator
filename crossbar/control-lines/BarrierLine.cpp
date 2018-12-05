#include "BarrierLine.h"

BarrierLine::BarrierLine(double value) {
	this->value = value;
}

double BarrierLine::get_value() const {
	return this->value;
}

void BarrierLine::set_value(double value) {
	this->value = value;
}

bool BarrierLine::is_up() const {
	return (this->get_value() == 0);
}

bool BarrierLine::is_down() const {
	return (this->get_value() == 1);
}

bool BarrierLine::is_open() const {
	return this->is_down();
}

bool BarrierLine::is_closed() const {
	return this->is_up();
}

void BarrierLine::open() {
	this->set_value(1);
}

void BarrierLine::close() {
	this->set_value(0);
}

void BarrierLine::toggle() {
	this->set_value(1 - this->get_value());
}
