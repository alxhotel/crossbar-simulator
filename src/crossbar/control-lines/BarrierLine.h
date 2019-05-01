#ifndef CROSSBAR_SIMULATOR_BARRIERLINE_H
#define CROSSBAR_SIMULATOR_BARRIERLINE_H

class BarrierLine {
public:
	typedef enum {
		BARRIER = 0,
		RF = 1
	} MODE;
	
	typedef enum {
		RAISED = 0,
		LOWERED = 1
	} STATE;
	
	BarrierLine(int mode, double state);
	
	BarrierLine(double state);

	int get_mode() const;

	void set_mode(int mode);
	
	double get_state() const;

	void set_state(double state);

	bool is_up() const;

	bool is_down() const;

	bool is_open() const;

	bool is_closed() const;

	void open();

	void close();

	void toggle();

private:
	int mode;
	double state;
};

#endif //CROSSBAR_SIMULATOR_BARRIERLINE_H
