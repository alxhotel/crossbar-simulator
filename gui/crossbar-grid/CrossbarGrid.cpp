#include <iostream>
#include <QString>
#include "CrossbarGrid.h"
#include "crossbar/CrossbarModel.h"

// Initialize constants
QPen CrossbarGrid::GRAY_PEN = QPen(QBrush(QColor(100, 100, 100)), 2);
QPen CrossbarGrid::RED_PEN = QPen(QBrush(QColor(200, 100, 100)), CrossbarGrid::PEN_WIDTH);
QPen CrossbarGrid::BLUE_PEN = QPen(QBrush(QColor(100, 100, 200)), CrossbarGrid::PEN_WIDTH);
QPen CrossbarGrid::RED_PEN_DASHED = QPen(QBrush(QColor(200, 100, 100)), CrossbarGrid::PEN_WIDTH, Qt::DotLine);
QPen CrossbarGrid::BLUE_PEN_DASHED = QPen(QBrush(QColor(100, 100, 200)), CrossbarGrid::PEN_WIDTH, Qt::DotLine);

/**
 * Constructor
 */
CrossbarGrid::CrossbarGrid(QWidget* parent, CrossbarModel* model) : QGraphicsView(parent) {
	this->model = model;
	this->scene = new QGraphicsScene(this);
	this->setScene(scene);
	
	// Set size of view
	int h_count, v_count, d_count;
	std::tie(this->m, this->n) = this->model->get_dimensions();
	std::tie(h_count, v_count, d_count) = this->model->get_control_line_dimensions();
	this->height = 2 * CrossbarGrid::OUTER_MARGIN + (this->m + 1) * CrossbarGrid::SQUARE_WIDTH;
	this->width = 2 * CrossbarGrid::OUTER_MARGIN + (this->n + 1) * CrossbarGrid::SQUARE_WIDTH;
	this->setFixedSize(this->width, this->height);

	// Control lines
	this->h_line_items = this->draw_h_lines(h_count);
	this->v_line_items = this->draw_v_lines(v_count);
	this->d_text_items = this->draw_d_lines(d_count);

	// Qubit positions
	this->qubit_items = {};
	for (auto const &entry : this->model->iter_qubits_positions()) {
		int q_id = entry.first;
		this->qubit_items[q_id] = new QubitCircle(q_id);
		this->scene->addItem(this->qubit_items[q_id]);
	}
	
	// Subscribe to all notifications
	this->model->subscribe(this);
}

/**
 * Draw the horizontal lines
 * @param count
 * @return map of the horizontal lines
 */
std::map<int, QGraphicsLineItem*> CrossbarGrid::draw_h_lines(int count) {
	std::map<int, QGraphicsLineItem*> line_items;
	int i = 0;
	for (int key = -1; key < count + 1; key++) {
		// Get line params
		int x1 = CrossbarGrid::OUTER_MARGIN;
		int y1 = CrossbarGrid::OUTER_MARGIN + (count + 1 - i) * CrossbarGrid::SQUARE_WIDTH + CrossbarGrid::SQUARE_WIDTH / 2;
		int x2 = this->width - CrossbarGrid::OUTER_MARGIN;
		int y2 = y1;

		// Draw line
		QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);
		line->setPen(CrossbarGrid::BLUE_PEN);
		this->scene->addItem(line);

		// Add button only for the real control lines
		if (key != -1 && key != count) {
			line_items.insert(std::pair<int, QGraphicsLineItem*>(key, line));
			LineTogglerCircle* circle = new LineTogglerCircle(x1, y1);
			circle->set_callback(key, this->model, &CrossbarModel::toggle_h_line);
			this->scene->addItem(circle);
		}
		i++;
	}
	
	return line_items;
}

/**
 * Draw the vertical lines
 * @param count
 * @return map of the vertical lines
 */
std::map<int, QGraphicsLineItem*> CrossbarGrid::draw_v_lines(int count) {
	std::map<int, QGraphicsLineItem*> line_items;
	int i = 0;
	for (int key = -1; key < count + 1; key++) {
		// Get line params
		int x1 = CrossbarGrid::OUTER_MARGIN + i * CrossbarGrid::SQUARE_WIDTH + CrossbarGrid::SQUARE_WIDTH / 2;
		int y1 = CrossbarGrid::OUTER_MARGIN;
		int x2 = x1;
		int y2 = this->height - CrossbarGrid::OUTER_MARGIN;

		// Draw line
		QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);
		line->setPen(CrossbarGrid::RED_PEN);
		this->scene->addItem(line);

		// Add button only for the real control lines
		if (key != -1 && key != count) {
			line_items[key] = line;
			LineTogglerCircle* circle = new LineTogglerCircle(x2, y2);
			circle->set_callback(key, this->model, &CrossbarModel::toggle_v_line);
			this->scene->addItem(circle);
		}
		i++;
	}
	
	return line_items;
}

/**
 * Draw the diagonal lines
 * @param count
 * @return map of the diagonal lines
 */
std::map<int, TextValueChanger*> CrossbarGrid::draw_d_lines(int count) {
	std::map<int, TextValueChanger*> value_items;
	int side = int((count - 1) / 2);
	int i = 0;
	for (int key = -1 * side; key < side + 1; key++) {
		int x1, y1, x2, y2;
		TextValueChanger* value_changer;
		if (i < count / 2) {
			x1 = CrossbarGrid::OUTER_MARGIN;
			y1 = CrossbarGrid::OUTER_MARGIN + (i + 2) * CrossbarGrid::SQUARE_WIDTH;
			x2 = CrossbarGrid::OUTER_MARGIN + (i + 2) * CrossbarGrid::SQUARE_WIDTH;
			y2 = CrossbarGrid::OUTER_MARGIN;
			value_changer = new TextValueChanger(x2 - CrossbarGrid::X_PADDING, y2 - CrossbarGrid::Y_PADDING);
		} else {
			x1 = CrossbarGrid::OUTER_MARGIN + (i + (1 - count) / 2) * CrossbarGrid::SQUARE_WIDTH;
			y1 = this->height - CrossbarGrid::OUTER_MARGIN;
			x2 = this->width - CrossbarGrid::OUTER_MARGIN;
			y2 = CrossbarGrid::OUTER_MARGIN + (i + (1 - count) / 2) * CrossbarGrid::SQUARE_WIDTH;
			value_changer = new TextValueChanger(x2, y2 - CrossbarGrid::Y_PADDING / 2);
		}

		// Draw line
		QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);
		line->setPen(CrossbarGrid::GRAY_PEN);
		this->scene->addItem(line);

		// Draw text value
		value_items[key] = value_changer;
		value_changer->set_callback(key, this->model, &CrossbarModel::change_d_line);
		this->scene->addItem(value_changer);
		i++;
	}
	
	return value_items;
}

/**
 * Handle the changes in the model
 */
void CrossbarGrid::notified() {
	// Repaint horizontal lines
	std::map<int, QGraphicsLineItem*>::iterator it_line;
	for (it_line = this->h_line_items.begin(); it_line != this->h_line_items.end(); it_line++) {
		QPen pen = (this->model->h_barrier_down(it_line->first)) ? CrossbarGrid::BLUE_PEN_DASHED : CrossbarGrid::BLUE_PEN;
		this->h_line_items[it_line->first]->setPen(pen);
	}
	// Repaint vertical lines
	for (it_line = this->v_line_items.begin(); it_line != this->v_line_items.end(); it_line++) {
		QPen pen = (this->model->v_barrier_down(it_line->first)) ? CrossbarGrid::RED_PEN_DASHED : CrossbarGrid::RED_PEN;
		this->v_line_items[it_line->first]->setPen(pen);
	}
	// Repaint diagonal lines
	std::map<int, TextValueChanger*>::iterator it_text;
	for (it_text = this->d_text_items.begin(); it_text != this->d_text_items.end(); it_text++) {
		this->d_text_items[it_text->first]->setPlainText(QString::number(this->model->d_line_value(it_text->first), 'd', 1));
	}
	// Repaint the qubits
	for (auto const &entry : this->model->iter_qubits_positions()) {
		int q_id = entry.first;
		QubitPosition* pos = entry.second->get_position();
		
		int y = CrossbarGrid::OUTER_MARGIN + (this->m - pos->get_i()) * CrossbarGrid::SQUARE_WIDTH;
		int x = CrossbarGrid::OUTER_MARGIN + (pos->get_j() + 1) * CrossbarGrid::SQUARE_WIDTH;
		this->qubit_items[q_id]->setPos(x, y);
	}
	
	this->update();
}