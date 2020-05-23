
class FuncNthPrime3:

    def __init__(self):
        self.input_size = 3
        self.output_size = 5

    def __call__(self, n):
        return [2, 3, 5, 7, 11, 13, 17, 19][n]
