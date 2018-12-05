#include <iostream>
#include <QApplication>

#include "crossbar/CrossbarModel.h"
#include "crossbar/CrossbarRouter.h"
#include "gui/MainWindow.h"

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	//MainWindow w(&listener);
    MainWindow mainWindow;
    mainWindow.show();
	//app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

	return app.exec();
}
