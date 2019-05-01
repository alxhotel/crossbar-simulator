#ifndef SETUPWINDOW_H
#define SETUPWINDOW_H

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <QStyle>
#include <QApplication>
#include <QDesktopWidget>
#include <QErrorMessage>
#include <QMainWindow>

#include "MainWindow.h"
#include "ui_SetupWindow.h"

namespace Ui {
	class SetupWindow;
}

class SetupWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit SetupWindow(QWidget* parent = 0);
	~SetupWindow();

private slots:
	void create_button_clicked();
	void load_json_clicked();
	
private:
	// GUI elements
	QWidget* parent;
	Ui::SetupWindow* ui;
	
	bool eventFilter(QObject* obj, QEvent* event);
	
	void draw_window();
	void open_main_window(int size, int data_qubits, int ancilla_qubits);
	void open_main_window(nlohmann::json topology);
	void show_alert(const char* type, const char* message);
};

#endif /* SETUPWINDOW_H */

