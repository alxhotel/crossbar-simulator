#ifndef CROSSBAR_SIMULATOR_TEXTVALUECHANGER_H
#define CROSSBAR_SIMULATOR_TEXTVALUECHANGER_H

#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include "crossbar/CrossbarModel.h"

/**
 * Value of a qubit line
 */
class TextValueChanger : public QGraphicsTextItem {
public:
	TextValueChanger(int x, int y) : QGraphicsTextItem() {
		this->setPos(x, y);
		this->setCursor(Qt::PointingHandCursor);
	}
			
	void set_callback(int data, CrossbarModel* model, void (CrossbarModel::* callback)(int, int(*)(int))) {
		this->data = data;
		this->model = model;
		this->callback = callback;
	}

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) {
		struct Inc {
			static int plus(int i) {
				return i + 1;
			}
			static int minus(int i) {
				return i - 1;
			}
		};
		
		if (event->button() == Qt::LeftButton) {
			(this->model->*(this->callback))(this->data, Inc::plus);
		} else if (event->button() == Qt::RightButton) {
			(this->model->*(this->callback))(this->data, Inc::minus);
		}
	}
	
private:
	int data;
	CrossbarModel* model;
	void (CrossbarModel::* callback)(int, int (*)(int));
};

#endif /* CROSSBAR_SIMULATOR_TEXTVALUECHANGER_H */

