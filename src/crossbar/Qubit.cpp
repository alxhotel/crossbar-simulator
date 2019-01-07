#include "Qubit.h"

Qubit::Qubit(QubitState* state, QubitPosition* position, bool is_ancillary) {
	this->state = state;
	this->position = position;
	this->original_position = position;
	this->is_ancillary = is_ancillary;
}

Qubit::~Qubit() {
	delete state;
	delete position;
	delete original_position;
}

QubitState* Qubit::get_state() const {
    return this->state;
}

void Qubit::set_state(QubitState* state) {
    this->state = state;
}

QubitPosition* Qubit::get_position() const {
    return this->position;
}

void Qubit::set_position(QubitPosition* position) {
    this->position = position;
}

bool Qubit::get_is_ancillary() const {
	return this->is_ancillary;
}

void Qubit::set_is_ancillary(bool is_ancillary) {
	this->is_ancillary = is_ancillary;
}
