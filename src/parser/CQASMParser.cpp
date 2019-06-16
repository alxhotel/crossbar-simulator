#include "CQASMParser.h"

/**
 * Parses a cQASM program.
 * @param text
 * @return 
 */
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

/**
 * Parses cQASM text and returns the number of qubits.
 * @param text
 * @return 
 */
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

/**
 * Translates from operations in "libqasm" to operations in the simulator.
 * @param operation
 * @param line_number
 * @return 
 */
Operation* CQASMParser::translate_operation(compiler::Operation* operation, int line_number) {
	// Make comparison case insensitive
	std::string gate_type = operation->getType();
	std::transform(gate_type.begin(), gate_type.end(),gate_type.begin(), ::tolower);

	//std::vector<long unsigned int> qubit_indices;
	std::vector<size_t> qubit_indices = operation->getQubitsInvolved().getSelectedQubits().getIndices();
	if (qubit_indices.size() == 0) {
		std::vector<size_t> first_qubit = operation->getTwoQubitPairs().first.getSelectedQubits().getIndices();
		std::vector<size_t> second_qubit = operation->getTwoQubitPairs().second.getSelectedQubits().getIndices();
		if (first_qubit.size() > 0) {
			qubit_indices.push_back(first_qubit[0]);
		}
		if (second_qubit.size() > 0) {
			qubit_indices.push_back(second_qubit[0]);
		}
	}
	
	// Shuttling
	if (gate_type == "shuttle_up") {
		return new Shuttling(Shuttling::DIR_UP, qubit_indices.front(), line_number);
	} else if (gate_type == "shuttle_down") {
		return new Shuttling(Shuttling::DIR_DOWN, qubit_indices.front(), line_number);
	} else if (gate_type == "shuttle_left") {
		return new Shuttling(Shuttling::DIR_LEFT, qubit_indices.front(), line_number);
	} else if (gate_type == "shuttle_right") {
		return new Shuttling(Shuttling::DIR_RIGHT, qubit_indices.front(), line_number);
	}
	
	// One-qubit gate: method z-gate
	else if (gate_type == "z_shuttle_left"
		|| gate_type == "s_shuttle_left" || gate_type == "t_shuttle_left"
		|| gate_type == "sdag_shuttle_left" || gate_type == "tdag_shuttle_left") {
		return new ShuttleGate(ShuttleGate::DIR_LEFT, qubit_indices.front(), line_number);
	} else if (gate_type == "z_shuttle_right"
		|| gate_type == "s_shuttle_right" || gate_type == "t_shuttle_right"
		|| gate_type == "sdag_shuttle_right" || gate_type == "tdag_shuttle_right") {
		return new ShuttleGate(ShuttleGate::DIR_RIGHT, qubit_indices.front(), line_number);
	}
	
	// One-qubit gate: method global
	else if (gate_type == "prep_x" || gate_type == "prep_y" || gate_type == "prep_z"
			|| gate_type == "i" || gate_type == "h"
			|| gate_type == "x" || gate_type == "y" || gate_type == "z"
			|| gate_type == "rx" || gate_type == "ry" || gate_type == "rz"
			|| gate_type == "x90" || gate_type == "y90" || gate_type == "mx90"
			|| gate_type == "my90" || gate_type == "s" || gate_type == "sdag"
			|| gate_type == "t" || gate_type == "tdag") {
		
		// TODO: add multiple direction
		return new SingleGate(gate_type, SingleGate::DIR_LEFT, qubit_indices.front(), line_number);
	}
	
	// Two-qubit gate: sqrt(SWAP)
	else if (gate_type == "sqswap") {
		return new SqSwap(qubit_indices.front(), qubit_indices.back(), line_number);
	} else if (gate_type == "cz") {
		return new CPhase(qubit_indices.front(), qubit_indices.back(), line_number);
	}
	
	// Measurement
	else if (gate_type == "measure_left_up") {
		return new Measurement(Measurement::DIR_ANCILLA_LEFT, Measurement::DIR_SITE_UP, qubit_indices.front(), line_number);
	} else if (gate_type == "measure_left_down") {
		return new Measurement(Measurement::DIR_ANCILLA_LEFT, Measurement::DIR_SITE_DOWN, qubit_indices.front(), line_number);
	} else if (gate_type == "measure_right_up") {
		return new Measurement(Measurement::DIR_ANCILLA_RIGHT, Measurement::DIR_SITE_UP, qubit_indices.front(), line_number);
	} else if (gate_type == "measure_right_down") {
		return new Measurement(Measurement::DIR_ANCILLA_RIGHT, Measurement::DIR_SITE_DOWN, qubit_indices.front(), line_number);
	}
	
	// Wait operation
	else if (gate_type == "wait") {
		return new Wait(operation->getWaitTime(), line_number);
	}
	
	// Error
	else {
		throw std::runtime_error(
			std::string("Gate `")
			+ gate_type
			+ std::string("` not supported at line ")
			+ std::to_string(line_number)
		);
	}
}
