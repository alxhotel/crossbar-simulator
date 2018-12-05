#ifndef CROSSBAR_SIMULATOR_CROSSBARGRID_H
#define CROSSBAR_SIMULATOR_CROSSBARGRID_H

#include <map>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include "LineTogglerCircle.h"
#include "QubitCircle.h"
#include "TextValueChanger.h"
#include "crossbar/CrossbarModel.h"
#include "crossbar/Subscriber.h"

class CrossbarGrid : public QGraphicsView, public Subscriber {
	Q_OBJECT
	
public:
	// Constructor
	CrossbarGrid(QWidget* parent, CrossbarModel* model);
	
	void notified();
	
private:
	// Constants
	static const int OUTER_MARGIN = 30;
	static const int SQUARE_WIDTH = 40;
	
	static const int Y_PADDING = 20;
	static const int X_PADDING = 15;
	
	static const int PEN_WIDTH = 3;
	static QPen GRAY_PEN;
	static QPen RED_PEN;
	static QPen BLUE_PEN;
	static QPen RED_PEN_DASHED;
	static QPen BLUE_PEN_DASHED;
	
	CrossbarModel* model;
	QGraphicsScene* scene;
	
	// Dimensions
	int m, n;
	int height, width;
	
	// Control lines
	std::map<int, QGraphicsLineItem*> h_line_items;
	std::map<int, QGraphicsLineItem*> v_line_items;
	std::map<int, TextValueChanger*> d_text_items;
	
	// Qubit positions
	std::map<int, QubitCircle*> qubit_items;
	
	// Draw methods
	std::map<int, QGraphicsLineItem*> draw_h_lines(int count);
	std::map<int, QGraphicsLineItem*> draw_v_lines(int count);
	std::map<int, TextValueChanger*> draw_d_lines(int count);
	
};

#endif /* CROSSBAR_SIMULATOR_CROSSBARGRID_H */
