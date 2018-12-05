#include "QubitLine.h"

QubitLine::QubitLine(double value) {
	this->value = value;
}

double QubitLine::get_value() const {
	return this->value;
}

void QubitLine::set_value(double value) {
	this->value = value;
}
