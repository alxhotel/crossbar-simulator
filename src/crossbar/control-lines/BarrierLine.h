#ifndef CROSSBAR_SIMULATOR_BARRIERLINE_H
#define CROSSBAR_SIMULATOR_BARRIERLINE_H

class BarrierLine {
public:
	BarrierLine(double value);

	double get_value() const;

	void set_value(double value);

	bool is_up() const;

	bool is_down() const;

	bool is_open() const;

	bool is_closed() const;

	void open();

	void close();

	void toggle();

private:
	double value;
};

#endif //CROSSBAR_SIMULATOR_BARRIERLINE_H
