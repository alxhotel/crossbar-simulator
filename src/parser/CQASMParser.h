#ifndef CQASMPARSER_H
#define CQASMPARSER_H

#include <string>
#include <iostream>
#include <stdexcept>
#include "libQasm.hpp"
#include "crossbar/operations/Operation.h"
#include "crossbar/operations/Shuttling.h"
#include "crossbar/operations/ShuttleGate.h"
#include "crossbar/operations/SingleGate.h"
#include "crossbar/operations/CPhase.h"
#include "crossbar/operations/SqSwap.h"
#include "crossbar/operations/Measurement.h"
#include "crossbar/operations/Wait.h"

class CQASMParser {
public:
	static std::vector<std::vector<Operation*> > parse(std::string text);
	
	static int get_num_qubits(std::string text);
	
private:
	static Operation* translate_operation(compiler::Operation* operation, int line_number);
};

#endif /* CQASMPARSER_H */

