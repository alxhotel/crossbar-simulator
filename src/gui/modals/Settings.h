#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "ui_Settings.h"

namespace Ui {
	class Settings;
}

class Settings : public QDialog {
	Q_OBJECT

public:
	static bool active_animation;
	static int speed;
	
	Settings(QWidget* parent);
	~Settings();
	
	void setAnimation(bool animation);
	void setAnimationSpeed(int speed);
	
public slots:
    void safe_speed_changed(int value);
	
private:
	QWidget* parent;
	Ui::Settings* ui;
	
	void draw_dialog();
};

#endif /* SETTINGS_H */

