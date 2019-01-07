#ifndef CROSSBAR_SIMULATOR_QUBITPOSITION_H
#define CROSSBAR_SIMULATOR_QUBITPOSITION_H

#include <vector>

class QubitPosition {
public:
	QubitPosition(int i, int j);
	
	int get_i();
	void set_i(int i);
	
	int get_j();
	void set_j(int j);

private:
	int i;
	int j;
};

#endif //CROSSBAR_SIMULATOR_QUBITPOSITION_H
