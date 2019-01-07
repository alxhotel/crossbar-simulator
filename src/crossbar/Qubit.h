#ifndef CROSSBAR_SIMULATOR_QUBIT_H
#define CROSSBAR_SIMULATOR_QUBIT_H

#include "QubitState.h"
#include "QubitPosition.h"

class Qubit {
public:
	Qubit(QubitState* state, QubitPosition* position, bool is_ancillary);
	~Qubit();
	
	QubitState* get_state() const;
	void set_state(QubitState* state);

	QubitPosition* get_position() const;
	void set_position(QubitPosition* position);
	
	QubitPosition* get_original_position() const;
	void set_original_position(QubitPosition* position);
	
	double get_dephasing_time() const;
	void set_dephasing_time(double dephasing_time);
	
	bool get_is_ancillary() const;
	void set_is_ancillary(bool is_ancillary);
	
private:
	QubitState* state;
	QubitPosition* position;
	QubitPosition* original_position;
	double dephasing_time;
	bool is_ancillary;
};


#endif //CROSSBAR_SIMULATOR_QUBIT_H
