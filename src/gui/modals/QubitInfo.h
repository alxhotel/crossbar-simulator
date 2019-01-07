#ifndef QUBITSTATES_H
#define QUBITSTATES_H

#include <string>
#include <QDialog>
#include <QString>
#include <QTableWidgetItem>
#include "crossbar/CrossbarModel.h"
#include "ui_QubitInfo.h"

namespace Ui {
	class QubitInfo;
}

class QubitInfo : public QDialog {
    Q_OBJECT

public:
    QubitInfo(QWidget* parent, CrossbarModel* model);
	~QubitInfo();

private:
	QWidget* parent;
	Ui::QubitInfo* ui;
	CrossbarModel* model;
	
	void draw_dialog();
};


#endif /* QUBITSTATES_H */

