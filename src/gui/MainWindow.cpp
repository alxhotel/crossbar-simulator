#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent, int size, int data_qubits, int ancilla_qubits) : QMainWindow(parent) {
	this->parent = parent;
	this->ui = new Ui::MainWindow();
	this->ui->setupUi(this);
	this->executorThread = new QThread();
	
	// Params for crossbar model
	this->model = new CrossbarModel(size, data_qubits, ancilla_qubits);
	
    // Center window
    QDesktopWidget* desktop = QApplication::desktop();
    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            desktop->availableGeometry()
        )
    );
    
	this->draw_window();
}

MainWindow::MainWindow(QWidget* parent, nlohmann::json topology) {
	this->parent = parent;
	this->ui = new Ui::MainWindow();
	this->ui->setupUi(this);
	this->executorThread = new QThread();
	
	int y_size = (int) topology["y_size"];
	int x_size = (int) topology["x_size"];
	this->model = new CrossbarModel(y_size, x_size,  0, 0);
	
	for (nlohmann::json::const_iterator it = topology["init_configuration"].begin();
			it != topology["init_configuration"].end(); ++it)
	{
		int q_id = std::stoi(it.key());
		std::string type = it.value()["type"];
		std::vector<int> value = it.value()["position"];
		int i = value[0];
		int j = value[1];
		model->add_qubit(q_id, new Qubit(
			(j % 2 == 0) ? new QubitState(0, 1) : new QubitState(1, 0),
			new QubitPosition(i, j),
			(type.compare("ancilla") == 0)
		));
	}
	
    // Center window
    QDesktopWidget* desktop = QApplication::desktop();
    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            desktop->availableGeometry()
        )
    );
    
	this->draw_window();
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::draw_window() {
	// Menu actions
	this->connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(new_configuration()));
	this->connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open_file()));
	this->connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(open_settings()));
	this->connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));

	// Evolve button
	this->connect(ui->evolveButton, SIGNAL(clicked()), this, SLOT(safe_evolve()));

	// Reset button
	this->connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(safe_reset()));
	
	// Check constraints button
	this->connect(ui->checkConstraintsButton, SIGNAL(clicked()), this, SLOT(safe_check_constraints()));
	
	// Run code button
	this->connect(ui->runCodeButton, SIGNAL(clicked()), this, SLOT(safe_run_code()));

	// Editor
	delete ui->editor;
	this->editor = new CodeEditor(this);
	this->ui->horizontalLayout_2->insertWidget(0, this->editor);

	// Visual crossbar grid
	delete ui->crossbarGrid;
	this->grid = new CrossbarGrid(ui->centralWidget, this->model);	
	this->ui->horizontalLayout_2->insertWidget(1, this->grid);
	this->connect(this->grid, SIGNAL(notified_signal()), this->grid, SLOT(notified_slot()));
	this->connect(this->grid, SIGNAL(notified_resize_signal()), this->grid, SLOT(notified_resize_slot()));
	this->grid->notified_resize();
	
	// Save model
	this->originalModel = this->model->clone();
}

void MainWindow::new_configuration() {
	this->parent->show();
	this->close();
}

void MainWindow::open_file() {
	QString fileName = QFileDialog::getOpenFileName(
		this,
        tr("Select a cQASM file"),
		"",
        tr("cQASM (*.qasm);;All Files (*)")
	);
	
	if (fileName.isEmpty()) {
		return;
	} else {
		QFile inputFile(fileName);
		
		if (!inputFile.open(QIODevice::ReadOnly)) {
			QMessageBox::information(
				this,
				tr("Unable to open file"),
				inputFile.errorString()
			);
			return;
		}
		
		QByteArray arr = inputFile.readAll();
		QString q_str(arr);
		
		this->editor->setPlainText(q_str);
		
		inputFile.close();
	}
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

void MainWindow::safe_reset() {
	delete this->model;
	this->model = this->originalModel;
	this->grid->setModel(this->model);
	this->originalModel = this->model->clone();
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
			this->show_alert("Message", this->ui->statusBar->currentMessage().toStdString().c_str());
		} else {
			this->show_alert("Error", this->ui->statusBar->currentMessage().toStdString().c_str());
		}
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
			bool is_valid = this->safe_check_code(operations);
			if (is_valid) {
				// Execute code
				this->safe_execute_code(operations);
			} else {
				this->show_alert("Error", this->ui->statusBar->currentMessage().toStdString().c_str());
			}
		} else {
			this->show_alert("Error", this->ui->statusBar->currentMessage().toStdString().c_str());
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
		// Ugly fix: replace number of qubits
		// And use "max + 1" to avoid an error in the parser
		int num_qubits = 1;
		for (std::pair<int, Qubit*> element : this->model->iter_qubits_positions()) {
			num_qubits = std::max(element.first + 1, num_qubits);
		}
		
		std::regex exp("qubits(\\s|\\t|\\n|\\r)*[0-9]+");
		text = std::regex_replace(
			text,
			exp,
			"qubits " + std::to_string(num_qubits)
		);
		
		std::vector<std::vector<Operation*> > operations = CQASMParser::parse(text);
		
		this->set_status("Parser", "VALID cQASM code");
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
bool MainWindow::safe_check_code(std::vector<std::vector<Operation*> > operations) {
	this->set_status("Checker", "Checking constraints...");
	try {
		// Check constraints
		CrossbarModel* cloned_model = this->model->clone();
		int line_number = ConstraintChecker::validate(cloned_model, operations);
		delete cloned_model;
		
		if (line_number == 0) {
			this->set_status("Checker", "VALID constraints");
			return true;
		} else {
			this->set_status("Checker", ("Conflict of constraints at line " + std::to_string(line_number - 1)).c_str());
			return false;
		}
	} catch (const std::exception& ex) {
		this->set_status("Checker", ex.what());
		return false;
	}
}

/**
 * Execute code
 */
void MainWindow::safe_execute_code(std::vector<std::vector<Operation*> > operations) {
	this->set_status("Executor", "Executing code...");
	try {
		std::cout << "Operations: " << operations.size() << std::endl << std::flush;
		
		Executor* executor = new Executor(
			this->model, Settings::active_animation, Settings::speed,
			this->editor, this->grid, operations
		);
		executor->moveToThread(this->executorThread);
		this->connect(this->executorThread, SIGNAL(started()), executor, SLOT(doWork()));
		this->connect(executor, SIGNAL(finished()), executor, SLOT(deleteLater()));
		this->connect(executor, SIGNAL(finished()), this->executorThread, SLOT(quit()));
		this->connect(executor, SIGNAL(cycle_done(int)), this, SLOT(cycle_done(int)));
		this->connect(executor, SIGNAL(finished_ok()), this, SLOT(finished_executing_code_ok()));
		this->connect(executor, SIGNAL(finished_err(const char*)),
				this, SLOT(finished_executing_code_err(const char*)));
		executorThread->start();
	} catch (const std::exception ex) {
		this->set_status("Executor", ex.what());
	}
}

/**
 * Slot as a callback of "safe_execute_code()"
 */
void MainWindow::finished_executing_code_ok() {
	this->set_status("Executor", "Code executed successfully");
}

void MainWindow::finished_executing_code_err(const char* message) {
	this->set_status("Executor", (std::string("Code executed with errors: ") + std::string(message)).c_str());
}

void MainWindow::cycle_done(int cycle) {
	this->set_status("Executor", (std::string("Cycle ") + std::to_string(cycle) + " done").c_str());
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
	
	bool isError = std::string(type).find("Error") != std::string::npos
		|| std::string(message).find("Conflict") != std::string::npos;
	if (isError) {
		QMessageBox::warning(
			this, 
			tr("Error"), 
			tr(message)
		);
	} else {
		QMessageBox::information(
			this, 
			tr("Message"), 
			tr(message)
		);
	}
}
