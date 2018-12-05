#ifndef CROSSBAR_SIMULATOR_LINETOGGLERCIRCLE_H
#define CROSSBAR_SIMULATOR_LINETOGGLERCIRCLE_H

#include <iostream>
#include <QBrush>
#include <QColor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>
#include "crossbar/CrossbarModel.h"

/**
 * Button to toggle the control lines
 */
class LineTogglerCircle : public QGraphicsEllipseItem {
	
	
public:
	LineTogglerCircle(int center_x, int center_y) : QGraphicsEllipseItem(
		center_x - LineTogglerCircle::RADIUS / 2,
		center_y - LineTogglerCircle::RADIUS / 2,
		LineTogglerCircle::RADIUS,
		LineTogglerCircle::RADIUS
	)  {
		this->setBrush(QBrush(QColor(200, 200, 200)));
		this->setCursor(Qt::PointingHandCursor);
	}
			
	void set_callback(int data, CrossbarModel* model, void (CrossbarModel::* callback)(int)) {
		this->data = data;
		this->model = model;
		this->callback = callback;
	}

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) {
       (this->model->*(this->callback))(this->data);
	}

private:
	static const int RADIUS = 12;
	
	int data;
	CrossbarModel* model;
	void (CrossbarModel::* callback)(int);
};

#endif /* CROSSBAR_SIMULATOR_LINETOGGLERCIRCLE_H */
