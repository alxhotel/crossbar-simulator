#include "QubitPosition.h"

QubitPosition::QubitPosition(int i, int j) {
	this->i = i;
	this->j = j;
}

int QubitPosition::get_i() {
	return this->i;
}

void QubitPosition::set_i(int i) {
	this->i = i;
}

int QubitPosition::get_j() {
	return this->j;
}

void QubitPosition::set_j(int j) {
	this->j = j;
}
