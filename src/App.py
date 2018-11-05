import sys
import signal
from PyQt5.QtWidgets import QApplication
from MainWindow import MainWindow
from Crossbar.CrossbarModel import CrossbarModel

"""
Main class that starts the simulator
"""
class App(QApplication):
	"""
	:param.n size of the crossbar
	"""
	def __init__(self, n):
		super(App, self).__init__([])
		self._main_window = MainWindow("Crossbar Simulator", CrossbarModel(n, n))
		self._main_window.show()

if __name__ == '__main__':
	# Initialize the simulator
	app = App(8)
	signal.signal(signal.SIGINT, signal.SIG_DFL)
	sys.exit(app.exec_())
