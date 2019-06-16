#include "SetupWindow.h"

SetupWindow::SetupWindow(QWidget* parent) : QMainWindow(parent) {
	this->parent = parent;
	this->ui = new Ui::SetupWindow();
	this->ui->setupUi(this);
	this->installEventFilter(this);
    
    // Center window
    QDesktopWidget* desktop = QApplication::desktop();
    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            desktop->availableGeometry()
        )
    );
    
	this->draw_window();
}

SetupWindow::~SetupWindow() {
	delete ui;
}

void SetupWindow::draw_window() {
	// Create button
	this->connect(ui->createButton, SIGNAL(clicked()), this, SLOT(create_button_clicked()));
	
	// Load JSON
	this->connect(ui->loadJSON, SIGNAL(clicked()), this, SLOT(load_json_clicked()));
}

void SetupWindow::open_main_window(int size, int data_qubits, int ancilla_qubits) {
	if (size < 2 || size > 10) {
		throw std::runtime_error("Size must be between 2 and 10");
	}

	if (data_qubits + ancilla_qubits > size * size) {
		throw std::runtime_error("The number of qubits does not fit inside the crossbar");
	}

	if (data_qubits < 0) {
		throw std::runtime_error("The number of data qubits must be bigger than 0");
	}

	if (ancilla_qubits < 0) {
		throw std::runtime_error("The number of ancilla qubits must be bigger than 0");
	}
		
	// Open main window
	MainWindow* mainWindow = new MainWindow(this, size, data_qubits, ancilla_qubits);
	mainWindow->show();

	this->hide();
}

void SetupWindow::open_main_window(nlohmann::json topology) {
	// Open main window
	MainWindow* mainWindow = new MainWindow(this, topology);
	mainWindow->show();

	this->hide();
}

void SetupWindow::create_button_clicked() {
	try {
		int size = (int) ui->sizeEdit->text().toInt();
		int data_qubits = (int) ui->dataQubitsEdit->text().toInt();
		int ancilla_qubits = (int) ui->ancillaQubitsEdit->text().toInt();

		this->open_main_window(size, data_qubits, ancilla_qubits);
	} catch (std::runtime_error e) {
		this->show_alert("Bad validation", e.what());
	}
}

void SetupWindow::load_json_clicked() {
	QString fileName = QFileDialog::getOpenFileName(
		this,
        tr("Select a JSON file"),
		"",
        tr("JSON (*.json);;All Files (*)")
	);
	
	if (fileName.isEmpty()) {
		return;
	} else {
		QFile inputFile(fileName);
		
		if (!inputFile.open(QIODevice::ReadOnly)) {
			QMessageBox::information(
				this,
				tr("Unable to open file"),
				inputFile.errorString()
			);
			return;
		}
		
		// Load JSON
		std::ifstream i(fileName.toStdString());
		nlohmann::json config;
		i >> config;
		
		if (config.count("topology") <= 0) {
			this->show_alert("Invalid file", "File does not have the topology field");
			return;
		}
		
		if (config["topology"].count("init_configuration") <= 0) {
			this->show_alert("Invalid file", "File does not have the init_configuration");
			return;
		}
		
		if (config["topology"].count("x_size") <= 0 || config["topology"].count("y_size") <= 0) {
			this->show_alert("Invalid file", "File does not have the size fields");
			return;
		}
		
		try {
			this->open_main_window(config["topology"]);
		} catch (std::runtime_error e) {
			// TODO
			this->show_alert("Bad validation", e.what());
		}
		
		inputFile.close();
	}
}

bool SetupWindow::eventFilter(QObject* obj, QEvent* event) {
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* key = static_cast<QKeyEvent*> (event);
		if ((key->key() == Qt::Key_Enter) || (key->key() == Qt::Key_Return)) {
			this->create_button_clicked();
		} else {
			return QObject::eventFilter(obj, event);
		}
		return true;
	} else {
		return QObject::eventFilter(obj, event);
	}
	return false;
}

void SetupWindow::show_alert(const char* type, const char* message) {
	std::cout << "[" << type << "] " << message << std::endl << std::flush;
	QMessageBox::warning(this, tr(type), tr(message));
}
