#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <iostream>
#include <QObject>
#include <QTimer>
#include <QWidget>
#include <interval-tree.h>

#include "crossbar/CrossbarModel.h"
#include "crossbar/operations/Operation.h"
#include "crossbar/ConstraintChecker.h"
#include "gui/crossbar-grid/CrossbarGrid.h"
#include "gui/editor/CodeEditor.h"

class Executor : public QObject {
	Q_OBJECT
public:
	Executor(CrossbarModel* model, bool active_animations,
		int speed, CodeEditor* editor, CrossbarGrid* grid,
		std::vector<std::vector<Operation*> > operations);
	~Executor();
	
signals:
	void finished();
	void finished_ok();
	void finished_err(const char* message);
	void cycle_done(int cycle);

public slots:
	void doWork();
	
private:
	CrossbarModel* model;
	bool active_animations;
	int speed;
	CodeEditor* editor;
	CrossbarGrid* grid;
	std::vector<std::vector<Operation*> > operations;
	
	double get_waiting_seconds(int speed);
};

#endif /* EXECUTOR_H */

