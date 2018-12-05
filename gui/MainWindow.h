#ifndef CROSSBAR_SIMULATOR_MAINWINDOW_H
#define CROSSBAR_SIMULATOR_MAINWINDOW_H

#include <QTimer>
#include <QMainWindow>
#include <QTextStream>
#include "crossbar/Matcher.h"
#include "crossbar/operations/Operation.h"
#include "crossbar/CrossbarModel.h"
#include "ui_MainWindow.h"
#include "modals/QubitInfo.h"
#include "editor/CodeEditor.h"
#include "gui/crossbar-grid/CrossbarGrid.h"

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
	void safe_run_code();
	void safe_validate_code();
	
private:
	// GUI elements
	QWidget* parent;
	Ui::MainWindow* ui;
	CrossbarGrid* grid;
	CodeEditor* editor;
	
	// Crossbar
	CrossbarModel* model;
	
	void draw_window();
};

#endif /* CROSSBAR_SIMULATOR_MAINWINDOW_H */
