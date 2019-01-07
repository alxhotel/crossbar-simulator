#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
	this->parent = parent;
	this->ui = new Ui::MainWindow();
	this->ui->setupUi(this);

	// Params for crossbar model
	this->num_ancillas = 0;
	this->model = new CrossbarModel(32);
	
	this->draw_window();
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::draw_window() {
	// Menu actions
	this->connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open_file()));
	this->connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(open_settings()));
	this->connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));

	// Evolve button
	this->connect(ui->evolveButton, SIGNAL(clicked()), this, SLOT(safe_evolve()));

	// Qubit States button
	this->connect(ui->qubitInfoButton, SIGNAL(clicked()), this, SLOT(open_qubit_info()));

	// Reset button
	this->connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(safe_reset()));
	
	// Check constraints button
	this->connect(ui->checkConstraintsButton, SIGNAL(clicked()), this, SLOT(safe_check_constraints()));
	
	// Run code button
	this->connect(ui->runCodeButton, SIGNAL(clicked()), this, SLOT(safe_run_code()));

	// Editor
	delete ui->editor;
	this->editor = new CodeEditor(this);
	ui->horizontalLayout_2->insertWidget(0, this->editor);

	// Visual crossbar grid
	delete ui->crossbarGrid;
	ui->crossbarGrid = new CrossbarGrid(ui->centralWidget, this->model);
	ui->horizontalLayout_2->insertWidget(1, ui->crossbarGrid);
}

void MainWindow::open_file() {
	// TODO: complete
	std::cout << "OPEN FILE" << std::endl << std::flush;
}

void MainWindow::open_settings() {
	Settings* settingsDialog = new Settings(this);
	settingsDialog->exec();
}

void MainWindow::exit() {
	this->close();
}

void MainWindow::safe_evolve() {
	try {
		this->model->evolve();
	} catch (const std::exception& ex) {
		// Undecidable configurations
		this->show_alert("Message", "Undecidable configuration found");
	} catch (...) {
		// Unknown error
		this->show_alert("Error", "Unknown error thrown in evolve");
	}
}

void MainWindow::open_qubit_info() {
	QubitInfo* qubitStates = new QubitInfo(this, this->model);
	qubitStates->exec();
}

void MainWindow::safe_reset() {
	this->model->reset();
}

/**
 * Validate the compatibility of the constraints
 */
void MainWindow::safe_check_constraints() {
	try {
		// Read code from editor
		std::string text = this->editor->getText();
		
		// Parse code
		std::vector<std::vector<Operation*> > operations = this->safe_parse_code(text);
		if (!operations.empty()) {
			// Check code
			this->safe_check_code(operations);
		}
		
		this->show_alert("Message", this->ui->statusBar->currentMessage().toStdString().c_str());
	} catch (...) {
		this->show_alert("Error", "Unknown error thrown in check_constraints");
	}
}

/**
 * Validate and run the code in the crossbar
 */
void MainWindow::safe_run_code() {
	try {
		// Read code from editor
		std::string text = this->editor->getText();
		
		// Parse code
		std::vector<std::vector<Operation*> > operations = this->safe_parse_code(text);
		if (!operations.empty()) {
			// Check code
			int line_number = this->safe_check_code(operations);
			if (line_number == 0) {
				// Execute code
				std::cout << "Operations: " << operations.size() << std::endl << std::flush;
				// TODO: Execute all the operations in a new thread
				this->safe_execute_code(operations);
				
				//std::thread execute_code_thread (&MainWindow::execute_code, this, operations);
				//execute_code_thread.join();
			}
		}
	} catch (...) {
		this->show_alert("Error", "Unknown error thrown in run_code");
	}
}

/**
 * Parse the code in the editor
 */
std::vector<std::vector<Operation*> > MainWindow::safe_parse_code(std::string text) {
	this->set_status("Parser", "Parsing code...");
	try {
		std::vector<std::vector<Operation*> > operations = CQASMParser::parse(text);
		this->set_status("Parser", "VALID cQASM code");
		
		// Reset model to new number of qubits
		int num_qubits = CQASMParser::get_num_qubits(text);
		this->model->resize(num_qubits);
		
		return operations;
	} catch (const std::exception& ex) {
		// TODO: show syntax error in code editor
		
		// Show parser error message
		this->set_status("Parser", ex.what());
		
		return {};
	}
}

/**
 * Check the constraints in the code
 */
int MainWindow::safe_check_code(std::vector<std::vector<Operation*> > operations) {
	this->set_status("Checker", "Checking constraints...");
	try {
		// Check constraints
		CrossbarModel* cloned_model = this->model->clone();
		int line_number = ConstraintChecker::validate(cloned_model, operations);
		delete cloned_model;
		if (line_number == 0) {
			this->set_status("Checker", "VALID constraints");
		} else {
			this->set_status("Checker", ("Conflict of constraints at line " + std::to_string(line_number - 1)).c_str());
		}
		return line_number;
	} catch (const std::exception& ex) {
		this->set_status("Checker", ex.what());
		return 0;
	}
}

/**
 * Execute code
 */
void MainWindow::safe_execute_code(std::vector<std::vector<Operation*> >operations) {
	this->set_status("Executor", "Executing code...");
	try {
		for (std::vector<Operation*> p_operation : operations) {
			for (Operation* operation : p_operation) {
				// TODO: highlight the current operation in editor
				this->editor->setHighlightGray(operation->getLineNumber());
				operation->execute(model, Settings::active_animation, Settings::speed);
			}
		}
		this->set_status("Executor", "Code executed successfully");
	} catch (const std::exception& ex) {
		this->set_status("Executor", ex.what());
	}
}

/**
 * Show a status message
 */
void MainWindow::set_status(const char* type, const char* message) {
	std::cout << "[" << type << "] " << message << std::endl << std::flush;
	this->ui->statusBar->showMessage(message);
}

/**
 * Show an alert popup
 */
void MainWindow::show_alert(const char* type, const char* message) {
	std::cout << "[" << type << "] " << message << std::endl << std::flush;
	QErrorMessage* error_dialog = new QErrorMessage(this);
	error_dialog->showMessage(message);
	error_dialog->exec();
}
