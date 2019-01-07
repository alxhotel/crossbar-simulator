#ifndef QUBITSTATE_H
#define QUBITSTATE_H

#include <complex> 

class QubitState {
public:
	
	QubitState(double alpha, double beta);
	
	std::complex<double> get_alpha();
	std::complex<double> get_beta();
	
	void set_beta(std::complex<double> beta);
	
	// TODO: finish implementation
	
	void rotate_phase();
	
	void x_gate();
	
	void z_gate();
	
	void h_gate();
	
	int measure();
	
	void reset();
	
private:
	std::complex<double> alpha;
	std::complex<double> beta;
	
	void normalize();
	
};

#endif /* QUBITSTATE_H */

