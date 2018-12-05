#include <iostream>
#include <regex>
#include <QErrorMessage>
#include "MainWindow.h"
#include "gui/modals/QubitInfo.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
	this->parent = parent;
	this->ui = new Ui::MainWindow();
	this->ui->setupUi(this);

	this->model = new CrossbarModel(8, 8);
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
	
	// Validate code button
	this->connect(ui->validateCodeButton, SIGNAL(clicked()), this, SLOT(safe_validate_code()));
	
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
	std::cout << "OPEN FILE" << std::endl << std::flush;
}

void MainWindow::open_settings() {
	std::cout << "SETTINGS" << std::endl << std::flush;
}

void MainWindow::exit() {
	this->close();
}

void MainWindow::safe_evolve() {
	try {
		this->model->evolve();
	} catch (const std::exception& ex) {
		// Undecidable configurations
		QErrorMessage* error_dialog = new QErrorMessage(this);
		error_dialog->showMessage(ex.what());
		error_dialog->exec();
		std::cout << "[Message] Undecidable configuration found" << std::endl;
	} catch (...) {
		// Unknown error
		QErrorMessage* error_dialog = new QErrorMessage(this);
		error_dialog->showMessage("Unknown error");
		error_dialog->exec();
		std::cout << "[Error] Unknown error thrown in evolve" << std::endl;
	}
}

void MainWindow::open_qubit_info() {
	QubitInfo* qubitStates = new QubitInfo(this, this->model);
	qubitStates->exec();
}

void MainWindow::safe_reset() {
	this->model->reset();
}

void MainWindow::safe_validate_code() {
	try {
		// TODO: validate code
		
		std::cout << "VALIDATE" << std::endl << std::flush;
		
	} catch (const std::exception& ex) {
		// Undecidable configurations
		QErrorMessage* error_dialog = new QErrorMessage(this);
		error_dialog->showMessage(ex.what());
		error_dialog->exec();
		std::cout << "[Message] Invalid instruction found" << std::endl;
	} catch (...) {
		//QErrorMessage(this) error_dialog;
		//error_dialog->setWindowModality(Qt.WindowModal);
		//error_dialog->showMessage("TEST");
		std::cout << "EXCEPTION THROWN in validate_code" << '\n';
	}
}

void MainWindow::safe_run_code() {
	try {
		// Read code from editor
		QString text = this->editor->toPlainText();
		QTextStream str(&text, QIODevice::ReadOnly);
		
		
		
		std::string line;
		for (int n = 1; !str.atEnd(); n++) {
			line = str.readLine().toUtf8().constData();

			// Match operation
			Operation* operation = Matcher::match(line);
			
			if (operation == NULL) {
				throw std::runtime_error("Invalid instruction at line " + std::to_string(n));
			} else {
				if (operation->validate_constraints()) {
					operation->execute();
				}
			}
		}
	} catch (const std::exception& ex) {
		// Undecidable configurations
		QErrorMessage* error_dialog = new QErrorMessage(this);
		error_dialog->showMessage(ex.what());
		error_dialog->exec();
		std::cout << "[Message] Invalid instruction found" << std::endl;
	} catch (...) {
		//QErrorMessage(this) error_dialog;
		//error_dialog->setWindowModality(Qt.WindowModal);
		//error_dialog->showMessage("TEST");
		std::cout << "EXCEPTION THROWN in run_code" << '\n';
	}
}
