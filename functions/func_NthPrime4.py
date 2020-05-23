
class FuncNthPrime4:

    def __init__(self):
        self.input_size = 4
        self.output_size = 6

    def __call__(self, n):
        return [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53][n]
