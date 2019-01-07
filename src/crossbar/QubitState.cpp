#include "QubitState.h"

QubitState::QubitState(double alpha, double beta) {
	this->alpha = std::complex<double>(alpha);
	this->beta = std::complex<double>(beta);
}

std::complex<double> QubitState::get_alpha() {
	return this->alpha;
}

std::complex<double> QubitState::get_beta() {
	return this->beta;
}

void QubitState::set_beta(std::complex<double> beta) {
	this->beta = beta;
}

void QubitState::rotate_phase() {

}

void QubitState::x_gate() {

}

void QubitState::z_gate() {

}

void QubitState::h_gate() {

}

int QubitState::measure() {
	double zero_prob = std::pow(abs(this->alpha), 2);
	// TODO: change random generator
	double random_measure = (rand() / double(RAND_MAX));

	// Collapse the state
	if (random_measure <= zero_prob) {
		this->alpha = 1;
		this->beta  = 0;
		return 0;
	} else {
		this->alpha = 0;
		this->beta  = 1;
		return 1;
	}
}

void QubitState::normalize() {
	
}

void QubitState::reset() {
	// TODO: redo this methods
	this->alpha = std::complex<double>(1);
	this->beta = std::complex<double>(0);
}
