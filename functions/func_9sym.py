
class Func9sym:

    def __init__(self):
        self.input_size = 9
        self.output_size = 1


    def __call__(self, n):
        popcnt = bin(n).count('1')
        return int(popcnt >= 3 and popcnt <= 6)
