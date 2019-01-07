#ifndef CROSSBAR_SIMULATOR_MAINWINDOW_H
#define CROSSBAR_SIMULATOR_MAINWINDOW_H

#include <regex>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <QThread>
#include <QMainWindow>
#include <QTextStream>
#include <QErrorMessage>
#include "parser/CQASMParser.h"
#include "crossbar/operations/Operation.h"
#include "crossbar/CrossbarModel.h"
#include "crossbar/ConstraintChecker.h"
#include "ui_MainWindow.h"
#include "modals/Settings.h"
#include "modals/QubitInfo.h"
#include "editor/CodeEditor.h"
#include "crossbar-grid/CrossbarGrid.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = 0);
	~MainWindow();

private slots:
	// Menu actions
	void open_file();
	void open_settings();
	void exit();
	
	// Buttons
	void safe_evolve();
	void open_qubit_info();
	void safe_reset();
	void safe_check_constraints();
	void safe_run_code();
	
private:
	// GUI elements
	QWidget* parent;
	Ui::MainWindow* ui;
	CrossbarGrid* grid;
	CodeEditor* editor;
	
	// Crossbar
	int num_ancillas;
	CrossbarModel* model;
	
	void draw_window();
	
	int safe_check_code(std::vector<std::vector<Operation*> > operations);
	std::vector<std::vector<Operation*> > safe_parse_code(std::string text);
	void safe_execute_code(std::vector<std::vector<Operation*> > operations);
	
	// Utils
	std::string get_editor_text();
	void set_status(const char* type, const char* message);
	void show_alert(const char* type, const char* message);
};

#endif /* CROSSBAR_SIMULATOR_MAINWINDOW_H */
