class QubitLine(object):
    def __init__(self, value = None):
        self._value = value

    def get_value(self):
        return self._value

    def set_value(self, value):
        self._value = value
