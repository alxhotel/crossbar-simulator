#include "QubitInfo.h"
#include <iomanip> // setprecision
#include <sstream>

QubitInfo::QubitInfo(QWidget* parent, CrossbarModel* model) : QDialog(parent) {
	this->parent = parent;
	this->ui = new Ui::QubitInfo();
    this->ui->setupUi(this);
	
	this->model = model;
	this->draw_dialog();
}

QubitInfo::~QubitInfo() {
	delete ui;
}

void QubitInfo::draw_dialog() {
	int i = 0;
	this->ui->tableWidget->insertRow(0);
	for (auto const &entry : this->model->iter_qubits_positions()) {
		int q_id = entry.first;
		QubitState* state = entry.second->get_state();
		
		std::stringstream labelStream;
		labelStream << "Q " << q_id;
		std::string label = labelStream.str();
		
		this->ui->tableWidget->setColumnCount(this->ui->tableWidget->columnCount() + 1);
		QTableWidgetItem* qtwi = new QTableWidgetItem(QString::fromStdString(label), QTableWidgetItem::Type);
		this->ui->tableWidget->setHorizontalHeaderItem(i, qtwi);
		
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << state->get_alpha().real();
		std::string alpha_str = ss.str();
		
		std::stringstream ss2;
		ss2 << std::fixed << std::setprecision(2) << state->get_beta().real();
		std::string beta_str = ss2.str();
		
		QTableWidgetItem* data = new QTableWidgetItem(QString::fromStdString(alpha_str + " |0> + " + beta_str + "|1>"), QTableWidgetItem::Type);
		this->ui->tableWidget->setItem(0, i, data);
		
		i++;
	}
}

