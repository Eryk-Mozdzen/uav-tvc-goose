
class Axis:
    def __init__(self, min, max):
        self.min = min
        self.max = max

    def get_offset(self):
        return 0.5*(self.min + self.max)

    def get_scale(self):
        offset = self.get_offset()
        centered = self.max - offset
        g = 9.81
        scale = g/centered
        return scale

axes = [
    Axis(min=- 9.77, max =10.01),
    Axis(min=- 9.87, max = 9.83),
    Axis(min=-10.24, max = 9.57)
]

offset = [a.get_offset() for a in axes]
scale  = [a.get_scale()  for a in axes]

print(offset)
print(scale)
