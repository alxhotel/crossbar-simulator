from Crossbar.QubitState import QubitState
from Crossbar.QubitLine import QubitLine
from Crossbar.BarrierLine import BarrierLine

# TODO:
# - change the number of RL lines, CL lines and qubit lines
# - Implement basic actions:
#   - Shuttling Hor. & Ver.
#   - One qubit gate
#   - Two qubit gate
#   - Measurement
class CrossbarModel(object):
    """
    Initialize the model
    :param.m	number of rows
    :param.n	number of columns
    """
    def __init__(self, m, n):
        self._m = m
        self._n = n
        # RL lines
        self._h_lines = {i:BarrierLine(0) for i in range(m - 1)}
        # CL lines
        self._v_lines = {i:BarrierLine(0) for i in range(n - 1)}
        # QL lines
        self._d_lines = {i:QubitLine(1.0 + (i % 2)) for i in range(-1 * (n - 1), m)}
        # Map[qubit  -> position]
        self._qubits_positions = {}
        # Map[position -> qubit]
        self._positions_qubits = {}
        # The states of the qubits
        self._qubit_states = {}
        # Initialize the maps
        q_id = 0
        for i in range(m):
            for j in range(n):
                if (i + j) % 2 == 0:
                    self._qubit_states[q_id] = QubitState(1, 0)
                    self._qubits_positions[q_id] = (i, j)
                    self._positions_qubits[(i, j)] = set([q_id])
                    q_id += 1
                else:
                    self._positions_qubits[(i, j)] = set([])
        # Set the subscribers
        self._subscribers = set()

    # Get the dimensions of the crossbar
    def get_dimensions(self):
        return (self._m, self._n)
    
    # Get the control line dimensions
    def get_control_line_dimensions(self):
        return (len(self._h_lines), len(self._v_lines), len(self._d_lines))

    def subscribe(self, subscriber):
        self._subscribers.add(subscriber)
        subscriber.notified()

    def notify_all(self):
        for sub in self._subscribers:
            sub.notified()

    def toggle_h_line(self, i):
        print("Toggle hor. " + str(i))
        self._h_lines[i].toggle()
        self.notify_all()

    def toggle_v_line(self, i):
        print("Toggle ver. " + str(i))
        self._v_lines[i].toggle()
        self.notify_all()

    def change_d_line(self, i, func):
        new_value = func(self._d_lines[i].get_value())
        print("QL[" + str(i) +  "] new value: " + str(new_value))
        self._d_lines[i].set_value(new_value)
        self.notify_all()

    def add_qubit(self, position):
        self._qubits_positions[position]

    def get_qubits_positions(self):
        return self._qubits_positions

    def h_barrier_up(self, i):
        if not i in self._h_lines:
            return True
        return self._h_lines[i].is_up()

    def v_barrier_up(self, i):
        if not i in self._v_lines:
            return True
        return self._v_lines[i].is_up()

    def h_barrier_down(self, i):
        if not i in self._h_lines:
            return False
        return self._h_lines[i].is_down()

    def v_barrier_down(self, i):
        if not i in self._v_lines:
            return False
        return self._v_lines[i].is_down()

    def d_line_value(self, i):
        return self._d_lines[i].get_value()

    def iter_qubits_positions(self):
        for q_id in self._qubits_positions:
            yield q_id, self._qubits_positions[q_id]

    # Check if the configuration is valid
    def check_valid_configuration(self):
        for q_id, (i, j) in self.iter_qubits_positions():
            d_line_top_val = self._d_lines[max((self._m - 1) * -1, j - i - 1)].get_value()
            d_line_middle_val = self._d_lines[j - i].get_value()
            d_line_bottom_val = self._d_lines[min(j - i + 1, (self._m - 1))].get_value()

            # Go upp or down / left or right at the same time
            if d_line_top_val > d_line_middle_val and d_line_bottom_val > d_line_middle_val or \
                d_line_top_val < d_line_middle_val and d_line_bottom_val < d_line_middle_val:
                if (self.h_barrier_down(i - 1) and self.h_barrier_down(i)) or \
                    (self.v_barrier_down(j - 1) and self.v_barrier_down(j)):
                    raise Exception("Undecidable configuration in (" + str(i) + ", " + str(j) + ")")

            # Go left/right and up/down at the same time
            if d_line_top_val > d_line_middle_val and (self.h_barrier_down(i) and self.v_barrier_down(j - 1)) or \
                d_line_bottom_val > d_line_middle_val and (self.h_barrier_down(i - 1) and self.v_barrier_down(j)):
                raise Exception("Undecidable configuration in (" + str(i) + ", " + str(j) + ")")

            count_barriers_down = int(self.h_barrier_down(i)) + int(self.h_barrier_down(i - 1)) \
                + int(self.v_barrier_down(j)) + int(self.v_barrier_down(j - 1))
            if count_barriers_down > 1:
                raise Exception("Undecidable configuration in (" + str(i) + ", " + str(j) + ")")
            
        return

    def evolve(self):
        # First, check any conflicts in the configuration
        self.check_valid_configuration()
        
        #new_qubits_positions = {}
        for q_id, (i, j) in self.iter_qubits_positions():
            d_line_top_val = self._d_lines[max((self._m - 1) * -1, j - i - 1)].get_value()
            d_line_middle_val = self._d_lines[j - i].get_value()
            d_line_bottom_val = self._d_lines[min(j - i + 1, (self._m - 1))].get_value()

            if d_line_top_val > d_line_middle_val:
                # Shuttle to the top
                if (i < self._m - 1) and self.h_barrier_down(i):
                    self._move_qubit(q_id, i + 1, j)
                # Shuttle to the left
                if j > 0 and self.v_barrier_down(j - 1):
                    self._move_qubit(q_id, i, j - 1)

            if d_line_bottom_val > d_line_middle_val:
                # Shuttle to the bottom
                if i > 0  and self.h_barrier_down(i - 1):
                    self._move_qubit(q_id, i - 1, j)
                # Shuttle to the right
                if j < (self._n - 1) and self.v_barrier_down(j):
                    self._move_qubit(q_id, i, j + 1)

            # Stay in the same site
            #if d_line_middle_val == d_line_top_val and d_line_middle_val == d_line_bottom_val:
            #    new_qubits_positions[q_id] = (i, j)

        self.notify_all()
    
    def _move_qubit(self, q_id, i_dest, j_dest):
        (i, j) = self._qubits_positions[q_id]
        self._qubits_positions[q_id] = (i_dest, j_dest)
        self._positions_qubits[(i, j)].remove(q_id)
        self._positions_qubits[(i_dest, j_dest)].add(q_id)
