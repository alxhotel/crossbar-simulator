#include <iostream>
#include <QStyle>
#include <QApplication>
#include <QDesktopWidget>

#include "crossbar/CrossbarModel.h"
#include "gui/SetupWindow.h"

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	//SetupWindow w(&listener);
    SetupWindow setupWindow;
    setupWindow.show();
    
	//app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

	return app.exec();
}
