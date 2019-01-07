#include "CQASMParser.h"

std::vector<std::vector<Operation*> > CQASMParser::parse(std::string text) {
	// Parse code
	compiler::QasmSemanticChecker* sm = new compiler::QasmSemanticChecker(text.c_str());
	if (sm->parseResult()) {
		throw std::runtime_error("Invalid cQASM code");
	}

	// Iterate over all operations
	std::vector<std::vector<Operation*> > operations = {};
	compiler::QasmRepresentation qasm_rep = sm->getQasmRepresentation();
	std::vector<compiler::SubCircuit> subcircuits = qasm_rep.getSubCircuits().getAllSubCircuits();
	for (compiler::SubCircuit subcircuit : subcircuits) {
		for (compiler::OperationsCluster* operation_cluster : subcircuit.getOperationsCluster()) {
			// TODD: remove for parallel operations
			if (operation_cluster->isParallel()) {
				throw std::runtime_error("Parallel operations are not supported");
			}

			std::vector<Operation*> p_operations = {};
			// for parallel operations
			for (compiler::Operation* compiler_operation : operation_cluster->getOperations()) {
				Operation* operation = CQASMParser::translate_operation(compiler_operation, operation_cluster->getLineNumber());
				p_operations.push_back(operation);
			}
			operations.push_back(p_operations);
		}
	}

	// Free mem
	delete sm;
	
	return operations;
}

int CQASMParser::get_num_qubits(std::string text) {
	// Parse code
	compiler::QasmSemanticChecker* sm = new compiler::QasmSemanticChecker(text.c_str());
	compiler::QasmRepresentation qasm_rep = sm->getQasmRepresentation();
	int num_qubits = qasm_rep.numQubits();
	if (sm->parseResult()) {
		throw std::runtime_error("Invalid cQASM code");
	}
	
	delete sm;
	
	return num_qubits;
}

Operation* CQASMParser::translate_operation(compiler::Operation* operation, int line_number) {
	std::vector<long unsigned int> qubit_indices = operation->getQubitsInvolved().getSelectedQubits().getIndices();
	if (qubit_indices.size() == 0) {
		qubit_indices.push_back(operation->getTwoQubitPairs().first.getSelectedQubits().getIndices()[0]);
		qubit_indices.push_back(operation->getTwoQubitPairs().second.getSelectedQubits().getIndices()[0]);
	}
	
	// Shuttling
	if (operation->getType() == "shuttle_up") {
		return new Shuttling(Shuttling::DIR_UP, qubit_indices.front(), line_number);
	} else if (operation->getType() == "shuttle_down") {
		return new Shuttling(Shuttling::DIR_DOWN, qubit_indices.front(), line_number);
	} else if (operation->getType() == "shuttle_left") {
		return new Shuttling(Shuttling::DIR_LEFT, qubit_indices.front(), line_number);
	} else if (operation->getType() == "shuttle_right") {
		return new Shuttling(Shuttling::DIR_RIGHT, qubit_indices.front(), line_number);
	}
	
	// One-qubit gate: method z-gate
	else if (operation->getType() == "z") {
		return new ZGate(qubit_indices.front(), line_number);
	}
	
	// TODO: One-qubit gate: method global
	
	// Two-qubit gate: sqrt(SWAP)
	else if (operation->getType() == "sqswap") {
		return new SqSwap(qubit_indices.front(), qubit_indices.back(), line_number);
	}
	
	// Measurement
	else if (operation->getType() == "measure_x" || operation->getType() == "measure_y"
			|| operation->getType() == "measure_z" || operation->getType() == "measure") {
		return new Measurement(qubit_indices.front(), line_number);
	}
	
	// Error
	else {
		throw std::runtime_error("Gate not supported");
	}
}
