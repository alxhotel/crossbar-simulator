class QubitState(object):
    """
    :param alpha    amplitude of |0>
    :param beta     amplitude of |1>
    """
    def __init__(self, alpha = 1, beta = 0):
        self.__alpha = complex(alpha)
        self.__beta = complex(beta)

    """Getter for alpha"""
    def alpha(self):
        return self.__alpha

    """Getter for beta"""
    def beta(self):
        return self.__beta

    """Chanhe phase"""
    def rotate_phase(self, phase):
        # TODO
        return

    """Apply a X gate"""
    def x_gate(self):
        self.__alpha, self.__beta = self.__alpha, self.__beta

    """Apply a Z gate"""
    def z_gate(self):
        self.__beta = -1 * self.__beta

    """Apply a Hadamard gate"""
    def h_gate(self):
        a = self.__alpha
        b = self.__beta
        self.__alpha = a + b
        self.__beta = a - b
        self.normalize()

    """Measure the qubit in the computational basis"""
    def measure(self):
        zero_prob = abs(self.__alpha) ** 2
        random_measure = random.random()
        
        # Collapse the state
        if random_measure < zero_prob:
            self.__alpha = complex(1)
            self.__beta  = complex(0)
            return 0
        else:
            self.__alpha = complex(0)
            self.__beta  = complex(1)
            return 1

    """Normalize the qubit state"""
    def normalize(self):
        factor = (abs(self.__alpha) ** 2 + abs(self.__beta) ** 2) ** 0.5
        self.__alpha /= factor
        self.__beta /= factor

    def __str__(self):
        return str(self.__alpha) + " |0> + " + str(self.__beta) + " |1>"

    def __eq__(self, other):
        assert isinstance(other, QubitState)
        comparison = (self.components - other.components).tolist()
        for num in comparison:
            if np.abs(num.imag) > 0.000001 or np.abs(num.real) > 0.000001:
                return False
        return True

