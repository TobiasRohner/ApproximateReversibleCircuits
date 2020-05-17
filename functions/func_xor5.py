
class FuncXor5:

    def __init__(self):
        self.input_size = 5
        self.output_size = 1


    def __call__(self, n):
        popcnt = bin(n).count('1')
        return popcnt % 2
