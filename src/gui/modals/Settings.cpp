#include "Settings.h"

bool Settings::active_animation = true;
int Settings::speed = 1;

Settings::Settings(QWidget* parent) : QDialog(parent) {
	this->parent = parent;
	this->ui = new Ui::Settings();
    this->ui->setupUi(this);
	
	this->draw_dialog();
}

Settings::~Settings() {
	delete ui;
}

void Settings::draw_dialog() {
	// Initialize slider
	this->ui->speedSlider->setSliderPosition(Settings::speed);
	
	// Listen for change
	this->connect(this->ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(safe_speed_changed(int)));
}

void Settings::safe_speed_changed(int value) {
	Settings::speed = value;
}

void Settings::setAnimation(bool animation) {
	Settings::active_animation = animation;
}

void Settings::setAnimationSpeed(int animation_speed) {
	Settings::speed = animation_speed;
}

