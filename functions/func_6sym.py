
class Func6sym:

    def __init__(self):
        self.input_size = 6
        self.output_size = 1

    def __call__(self, n):
        popcnt = bin(n).count('1')
        return int(popcnt==2 or popcnt==3 or popcnt==4)
