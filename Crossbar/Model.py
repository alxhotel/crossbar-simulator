from Crossbar.QubitState import QubitState
from Crossbar.QubitLine import QubitLine
from Crossbar.BarrierLine import BarrierLine

# TODO:
# - change the number of RL lines, CL lines
# - fix the shuttling
class CrossbarModel(object):
    """
    Initialize the model
    :param.m	number of rows
    :param.n	number of columns
    """
    def __init__(self, m, n):
        # RL lines
        self._h_lines = [BarrierLine(0) for _ in range(m + 1)]
        # CL lines
        self._v_lines = [BarrierLine(0) for _ in range(n + 1)]
        # QL lines
        self._d_lines = [QubitLine(1.0 + (i + 1) % 2) for i in range(m + n - 1)]
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
                    self._qubits_positions[q_id] = (i, j)
                    self._positions_qubits[(i, j)] = set([q_id])
                    self._qubit_states[q_id] = QubitState(1, 0)
                    q_id += 1
                else:
                    self._positions_qubits[(i, j)] = set([])
        # Set the subscribers
        self._subscribers = set()

    def get_line_dimensions(self):
        return len(self._h_lines), len(self._v_lines), len(self._d_lines)

    def subscribe(self, subscriber):
        self._subscribers.add(subscriber)
        subscriber.notified()

    def notify_all(self):
        for sub in self._subscribers:
            sub.notified()

    def toggle_h_line(self, i):
        if not i in {0, len(self._h_lines) - 1}:
            self._h_lines[i].toggle()
        self.notify_all()

    def toggle_v_line(self, i):
        if not i in {0, len(self._v_lines) - 1}:
            self._v_lines[i].toggle()
        self.notify_all()

    def change_d_line(self, i, f):
        new_value = f(self._d_lines[i].get_value())
        self._d_lines[i].set_value(new_value)
        self.notify_all()

    def add_qubit(self, position):
        self._qubits_positions[position]

    def get_qubits_positions(self):
        return self._qubits_positions

    def h_barrier_up(self, i):
        return self._h_lines[i].is_up()

    def v_barrier_up(self, i):
        return self._v_lines[i].is_up()

    def h_barrier_down(self, i):
        return self._h_lines[i].is_down()

    def v_barrier_down(self, i):
        return self._v_lines[i].is_down()

    def d_line_value(self, i):
        return self._d_lines[i].get_value()

    def iter_qubits_positions(self):
        for q_id in self._qubits_positions:
            yield q_id, self._qubits_positions[q_id]

    def evolve(self):
        new_qubits_positions = {}
        for q_id in self._qubits_positions:
            (i, j) = self._qubits_positions[q_id]
            d_line_top_val = self._d_lines[max(0, i + j - 1)].get_value()
            d_line_middle_val = self._d_lines[i + j].get_value()
            d_line_bottom_val = self._d_lines[min(i + j + 1, len(self._d_lines) - 1)].get_value()

            if d_line_top_val > d_line_middle_val and d_line_bottom_val > d_line_middle_val or \
                d_line_top_val < d_line_middle_val and d_line_bottom_val < d_line_middle_val:
                if not (self.h_barrier_up(i) or self.h_barrier_up(i + 1)) or \
                    not (self.v_barrier_up(j) or self.v_barrier_up(j + 1)):
                    raise Exception("Undecidable configuration")

            if d_line_top_val > d_line_middle_val and not (self.h_barrier_up(i) or self.v_barrier_up(j)) or \
                d_line_bottom_val > d_line_middle_val and not (self.h_barrier_up(i + 1) or self.v_barrier_up(j + 1)):
                raise Exception("Undecidable configuration")

            if d_line_top_val > d_line_middle_val:
                # Shuttle to the top
                if self.h_barrier_down(i):
                    new_qubits_positions[q_id] = (i - 1, j)
                    self._positions_qubits[(i, j)].remove(q_id)
                    self._positions_qubits[(i - 1, j)].add(q_id)
                # Shuttle to the left
                if self.v_barrier_down(j):
                    new_qubits_positions[q_id] = (i, j - 1)
                    self._positions_qubits[(i, j)].remove(q_id)
                    self._positions_qubits[(i, j - 1)].add(q_id)

            if d_line_bottom_val > d_line_middle_val:
                # Shuttle to the bottom
                if self.h_barrier_down(i + 1):
                    new_qubits_positions[q_id] = (i + 1, j)
                    self._positions_qubits[(i, j)].remove(q_id)
                    self._positions_qubits[(i + 1, j)].add(q_id)
                # Shuttle to the right
                if self.v_barrier_down(j + 1):
                    new_qubits_positions[q_id] = (i, j + 1)
                    self._positions_qubits[(i, j)].remove(q_id)
                    self._positions_qubits[(i, j + 1)].add(q_id)

            # Stay in the same site
            if d_line_middle_val == d_line_top_val and d_line_middle_val == d_line_bottom_val:
                new_qubits_positions[q_id] = (i, j)

        self._qubits_positions = new_qubits_positions
        self.notify_all()
