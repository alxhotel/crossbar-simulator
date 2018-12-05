#ifndef CROSSBAR_SIMULATOR_QUBITCIRCLE_H
#define CROSSBAR_SIMULATOR_QUBITCIRCLE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>

/**
 * A qubit
 */
class QubitCircle : public QGraphicsEllipseItem {
	
	
public:
	// Constructor
	QubitCircle(int q_id) : QGraphicsEllipseItem(0, 0, QubitCircle::RADIUS, QubitCircle::RADIUS) {
		this->setBrush(QBrush(QColor(100, 200, 100, 200)));
		QGraphicsTextItem* text = new QGraphicsTextItem(QString::number(q_id), this);
        if (q_id <= 9) {
            text->setPos(2, -3);
		} else {
            text->setPos(-2, -3);
		}
	}
	
	void setPos(int x, int y) {
		QGraphicsEllipseItem::setPos(x - QubitCircle::RADIUS / 2, y - QubitCircle::RADIUS / 2);
	}
	
private:
	static const int RADIUS = 20;
};

#endif /* CROSSBAR_SIMULATOR_QUBITCIRCLE_H */

