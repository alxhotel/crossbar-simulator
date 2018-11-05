class BarrierLine(object):
    def __init__(self, value = 0):
        self._value = value

    def get_value(self):
        return self._value

    def set_value(self, value):
        self._value = value

    def toggle(self):
        self._value = 1 - self._value

    def is_up(self):
        return not(self.is_down())

    def is_down(self):
        return bool(self._value)
