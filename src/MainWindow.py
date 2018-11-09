from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QMainWindow, QHBoxLayout, QWidget, QPushButton, QErrorMessage
from CrossbarGrid import CrossbarGrid

"""
Class that generates the GUI and handles the button events
"""
class MainWindow(QMainWindow):
    """
    :param.title    title of the main window
    :param.model    a crossbar model instance
    """
    def __init__(self, title, model):
        super(MainWindow, self).__init__()
        self.setWindowTitle(title)
        self._model = model
        self._draw_window()

    def _draw_window(self):
        central_widget = QWidget(self)

        # Crossbar layout
        grid = CrossbarGrid(central_widget, self._model)

        # Evolve button
        evolve_button = QPushButton("Evolve", self)
        evolve_button.clicked.connect(self._safe_evolve)

        # Run code
        #run_code_button = QPushButton("Run code", self)
        #run_code_butto.clicked.connect(self._safe_run_code)

        layout = QHBoxLayout()
        layout.addWidget(grid)
        layout.addWidget(evolve_button)

        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

    """
    Simulate the next time-step of the crossbar
    """
    def _safe_evolve(self):
        try:
            self._model.evolve()
        except Exception as e:
            print(e.args)
            error_dialog = QErrorMessage(self)
            error_dialog.setWindowModality(Qt.WindowModal)
            error_dialog.showMessage(e.args[0])
	
    """
    Simulate the QASM code in the crossbar
    """
    # TODO
    def _safe_run_code(self):
        try:
            self._model.evolve()
        except Exception as e:
            print(e.args)
