#!/usr/bin/env -S uv run

# /// script
# dependencies = [
#     "pandas",
#     "matplotlib",
# ]
# ///

import pandas as pd
import matplotlib.pyplot as plt

file = pd.read_csv('output.csv')

plt.figure()
plt.plot(file['time'], file['pz'])
plt.grid()

plt.figure()
plt.plot(file['time'], file['accel_z'])
plt.grid()

plt.show()
