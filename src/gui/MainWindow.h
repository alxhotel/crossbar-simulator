#ifndef CROSSBAR_SIMULATOR_MAINWINDOW_H
#define CROSSBAR_SIMULATOR_MAINWINDOW_H

#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <QString>
#include <QThread>
#include <QMainWindow>
#include <QTextStream>
#include <QErrorMessage>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QByteArray>

#include "parser/CQASMParser.h"
#include "crossbar/operations/Operation.h"
#include "crossbar/CrossbarModel.h"
#include "crossbar/ConstraintChecker.h"
#include "ui_MainWindow.h"
#include "modals/Settings.h"
#include "modals/QubitInfo.h"
#include "editor/CodeEditor.h"
#include "crossbar-grid/CrossbarGrid.h"
#include "Executor.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = 0, int size = 4, int data_qubits = 4, int ancilla_qubits = 4);
	explicit MainWindow(QWidget* parent, nlohmann::json topology);
	~MainWindow();

private slots:
	// Menu actions
	void new_configuration();
	void open_file();
	void open_settings();
	void exit();
	
	// Buttons
	void safe_evolve();
	void safe_reset();
	void safe_check_constraints();
	void safe_run_code();
	
	// Executing code
	void finished_executing_code_ok();
	void finished_executing_code_err(const char* message);
	void cycle_done(int cycle);
	
private:
	// GUI elements
	QWidget* parent;
	Ui::MainWindow* ui;
	CrossbarGrid* grid;
	CodeEditor* editor;
	QThread* executorThread = NULL;
	
	// Crossbar
	CrossbarModel* model = NULL;
	CrossbarModel* originalModel;
	
	void draw_window();
	
	bool safe_check_code(std::vector<std::vector<Operation*> > operations);
	std::vector<std::vector<Operation*> > safe_parse_code(std::string text);
	void safe_execute_code(std::vector<std::vector<Operation*> > operations);
	
	// Utils
	std::string get_editor_text();
	void set_status(const char* type, const char* message);
	void show_alert(const char* type, const char* message);
};

#endif /* CROSSBAR_SIMULATOR_MAINWINDOW_H */
