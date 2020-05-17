
class Func5mod5:

    def __init__(self):
        self.input_size = 5
        self.output_size = 1


    def __call__(self, n):
        return int(n % 5 == 0)
