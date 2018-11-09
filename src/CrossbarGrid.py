from PyQt5.QtWidgets import QGraphicsView, QGraphicsScene, QGraphicsLineItem, QGraphicsEllipseItem, QGraphicsTextItem
from PyQt5.QtGui import QPen, QBrush, QColor
from PyQt5.QtCore import Qt

# Button to toggle the control lines
class LineTogglerCircle(QGraphicsEllipseItem):
    __RADIUS = 12

    def __init__(self, center_x, center_y, callback, data):
        super(LineTogglerCircle, self).__init__(
            center_x - self.__RADIUS / 2,
            center_y - self.__RADIUS / 2,
            self.__RADIUS,
            self.__RADIUS
        )
        self.__callback = callback
        self.__data = data
        self.setBrush(QBrush(QColor(200, 200, 200)))
        self.setCursor(Qt.PointingHandCursor)

    def mousePressEvent(self, event):
        super(LineTogglerCircle, self).mousePressEvent(event)
        self.__callback(self.__data)

# A qubit
class QubitCircle(QGraphicsEllipseItem):
    __RADIUS = 20

    def __init__(self, q_id):
        super(QubitCircle, self).__init__(0, 0, self.__RADIUS, self.__RADIUS)
        self.setBrush(QBrush(QColor(100, 200, 100, 200)))
        text = QGraphicsTextItem(str(q_id), self)
        if q_id <= 9:
            text.setPos(2, -3)
        else:
            text.setPos(-2, -3)

    def setPos(self, x, y):
        super(QubitCircle, self).setPos(x - self.__RADIUS / 2, y - self.__RADIUS / 2)

# Value of a qubit line
class TextValueChanger(QGraphicsTextItem):
    def __init__(self, x, y):
        super(TextValueChanger, self).__init__()
        self.setPos(x, y)
        self.setCursor(Qt.PointingHandCursor)
        self.__callback = None
        self.__data = None

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            inc = lambda i: i + 1
            self.__callback(self.__data, inc)
        elif event.button() == Qt.RightButton:
            dec = lambda i: i - 1
            self.__callback(self.__data, dec)

    def set_callbacks(self, callback, data):
        self.__callback = callback
        self.__data = data

# Crossbar layout
class CrossbarGrid(QGraphicsView):
    __OUTER_MARGIN = 30
    __SQUARE_WIDTH = 40

    __Y_PADDING = 20
    __X_PADDING = 15

    __PEN_WIDTH = 3
    __GRAY_PEN = QPen(QBrush(QColor(100, 100, 100)), 2)
    __RED_PEN = QPen(QBrush(QColor(200, 100, 100)), __PEN_WIDTH)
    __BLUE_PEN = QPen(QBrush(QColor(100, 100, 200)), __PEN_WIDTH)
    __RED_PEN_DASHED = QPen(QBrush(QColor(200, 100, 100)), __PEN_WIDTH, Qt.DotLine)
    __BLUE_PEN_DASHED = QPen(QBrush(QColor(100, 100, 200)), __PEN_WIDTH, Qt.DotLine)

    def __init__(self, parent, model):
        super(CrossbarGrid, self).__init__(parent)
        self.__model = model
        self.__scene = QGraphicsScene(self)
        self.setScene(self.__scene)
        
        # Set size of view
        (self.__m, self.__n) = model.get_dimensions()
        (h_count, v_count, d_count) = model.get_control_line_dimensions()
        self.__height = 2 * self.__OUTER_MARGIN + (self.__m + 1) * self.__SQUARE_WIDTH
        self.__width = 2 * self.__OUTER_MARGIN + (self.__n + 1) * self.__SQUARE_WIDTH
        self.setFixedSize(self.__width, self.__height)
        
        # Controll lines
        self.__h_line_items = self.__draw_h_lines(h_count)
        self.__v_line_items = self.__draw_v_lines(v_count)
        self.__d_values = self.__draw_d_lines(d_count)
        
        # Qubit positions
        self.__qubits = {}
        for q_id, _ in self.__model.iter_qubits_positions():
            self.__qubits[q_id] = QubitCircle(q_id)
            self.__scene.addItem(self.__qubits[q_id])
        
        # Subscibe to all notifications
        self.__model.subscribe(self)

    def __draw_h_lines(self, count):
        line_items = {}
        i = 0
        for key in range(-1, count + 1):
            x1 = self.__OUTER_MARGIN
            y1 = self.__OUTER_MARGIN + (count + 1 - i) * self.__SQUARE_WIDTH + self.__SQUARE_WIDTH / 2
            x2 = self.__width - self.__OUTER_MARGIN
            y2 = y1

            line = QGraphicsLineItem(x1, y1, x2, y2)
            line.setPen(self.__BLUE_PEN)
            self.__scene.addItem(line)
            
            # Add button only for the real cotrol lines
            if not key in {-1, count}:
                line_items[key] = line
                circle = LineTogglerCircle(x1, y1, self.__model.toggle_h_line, key)
                self.__scene.addItem(circle)
            i += 1
        return line_items

    def __draw_v_lines(self, count):
        line_items = {}
        i = 0
        for key in range(-1, count + 1):
            x1 = self.__OUTER_MARGIN + i * self.__SQUARE_WIDTH + self.__SQUARE_WIDTH / 2
            y1 = self.__OUTER_MARGIN
            x2 = x1
            y2 = self.__height - self.__OUTER_MARGIN

            line = QGraphicsLineItem(x1, y1, x2, y2)
            line.setPen(self.__RED_PEN)
            self.__scene.addItem(line)
            
            # Add button only for the real cotrol lines
            if not key in {-1, count}:
                line_items[key] = line
                circle = LineTogglerCircle(x2, y2, self.__model.toggle_v_line, key)
                self.__scene.addItem(circle)
            i += 1
        return line_items

    def __draw_d_lines(self, count):
        value_items = {}
        side = int((count - 1) / 2)
        i = 0
        for key in range(-1 * side, side + 1):
            if i <= count / 2:
                x1 = self.__OUTER_MARGIN
                y1 = self.__OUTER_MARGIN + (i + 2) * self.__SQUARE_WIDTH
                x2 = self.__OUTER_MARGIN + (i + 2) * self.__SQUARE_WIDTH
                y2 = self.__OUTER_MARGIN
                value_changer = TextValueChanger(x2 - self.__X_PADDING, y2 - self.__Y_PADDING)
            else:
                x1 = self.__OUTER_MARGIN + (i + (1 - count) / 2) * self.__SQUARE_WIDTH
                y1 = self.__height - self.__OUTER_MARGIN
                x2 = self.__width - self.__OUTER_MARGIN
                y2 = self.__OUTER_MARGIN + (i + (1 - count) / 2) * self.__SQUARE_WIDTH
                value_changer = TextValueChanger(x2, y2 - self.__Y_PADDING / 2)
            line = QGraphicsLineItem(x1, y1, x2, y2)
            line.setPen(self.__GRAY_PEN)
            self.__scene.addItem(line)
            
            value_items[key] = value_changer
            value_changer.set_callbacks(self.__model.change_d_line, key)
            self.__scene.addItem(value_changer)
            i += 1
        return value_items

    # Handle the changes in the model
    def notified(self):
        for i in self.__h_line_items:
            pen = self.__BLUE_PEN_DASHED if self.__model.h_barrier_down(i) else self.__BLUE_PEN
            self.__h_line_items[i].setPen(pen)
        for i in self.__v_line_items:
            pen = self.__RED_PEN_DASHED if self.__model.v_barrier_down(i) else self.__RED_PEN
            self.__v_line_items[i].setPen(pen)
        for i in self.__d_values:
            self.__d_values[i].setPlainText(str(self.__model.d_line_value(i)))
        for q_id, (i, j) in self.__model.iter_qubits_positions():
            y = self.__OUTER_MARGIN + (self.__m - i) * self.__SQUARE_WIDTH
            x = self.__OUTER_MARGIN + (j + 1) * self.__SQUARE_WIDTH
            self.__qubits[q_id].setPos(x, y)
        self.update()
