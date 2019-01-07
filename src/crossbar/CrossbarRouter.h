#ifndef CROSSBAR_SIMULATOR_CROSSBARROUTER_H
#define CROSSBAR_SIMULATOR_CROSSBARROUTER_H


#include "CrossbarModel.h"

class CrossbarRouter {
public:
    CrossbarRouter(CrossbarModel *model);

private:
    CrossbarModel *p_model;

    std::vector<Qubit> m_qubits;
    std::vector< std::pair<int, int> > m_positions;
};


#endif //CROSSBAR_SIMULATOR_CROSSBARROUTER_H
