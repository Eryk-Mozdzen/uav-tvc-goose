
def get_calib_data(acc):
    offset = [
        0.5*(acc[0][0] + acc[0][1]),
        0.5*(acc[1][0] + acc[1][1]),
        0.5*(acc[2][0] + acc[2][1])
    ]

    acc_centered = [
        [acc[0][0] - offset[0], acc[0][1] - offset[0]],
        [acc[1][0] - offset[1], acc[1][1] - offset[1]],
        [acc[2][0] - offset[2], acc[2][1] - offset[2]]
    ]

    g = 9.81

    scale = [
        g/acc_centered[0][1],
        g/acc_centered[1][1],
        g/acc_centered[2][1]
    ]

    return offset, scale

offset, scale = get_calib_data([
    [- 9.77, 10.01],
    [- 9.89,  9.84],
    [-10.24,  9.67],
])

print(offset)
print(scale)