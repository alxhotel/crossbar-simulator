#ifndef CROSSBAR_SIMULATOR_QUBITLINE_H
#define CROSSBAR_SIMULATOR_QUBITLINE_H

class QubitLine {
public:
	QubitLine(double value);

	double get_value() const;

	void set_value(double value);

private:
	double value;
};

#endif //CROSSBAR_SIMULATOR_QUBITLINE_H
