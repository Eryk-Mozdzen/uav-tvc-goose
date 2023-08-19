import numpy as np

logic_ones = [
    1.841,
    1.847,
    1.853,
    1.850,
    1.851,
    1.850,
    1.843
]

logic_zeros = [
    1.672,
    1.691,
    1.678,
    1.700,
    1.678,
    1.678,
    1.678
]

smallest_one = min(logic_ones)
biggest_zero = max(logic_zeros)

print(f'smallest 1: {smallest_one}')
print(f'biggest  0: {biggest_zero}')

center = (smallest_one + biggest_zero)/2
gap = smallest_one - biggest_zero

print(f'center: {center}')
print(f'gap:    {gap}')

histeresis_width = 0.5*gap

print(f'hist width: {histeresis_width}')
