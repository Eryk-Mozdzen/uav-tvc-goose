import numpy as np
import pandas as pd
import time
import socket
import os

file = pd.read_csv('output.csv')

timestamps = file['t'].values
poses = np.array([
    file['x'].values,
    file['y'].values,
    file['z'].values,
    file['phi'].values,
    file['theta'].values,
    file['psi'].values,
]).transpose()

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as soc:
    soc.connect(('127.0.0.1', 8080))

    def write(msg):
        soc.sendall(msg.encode())

    path = os.path.dirname(__file__)

    write('clear\n')
    write(f'create com model transform translation 0 0 -0.05 material color 192 192 192 geometry {path}/goose.stl\n')
    write('create column1 cylinder transform rpy 90 0 0 translation  2 0 1 material color 255 127 0 geometry 0.1 2 0.1\n')
    write('create column2 cylinder transform rpy 90 0 0 translation -2 0 1 material color 255 127 0 geometry 0.1 2 0.1\n')

    start = time.time()

    def elapsed():
        return (time.time() - start)

    while elapsed()<timestamps[-1]:
        best_dt = 100
        for t, p in zip(timestamps, poses):
            dt = np.abs(t - elapsed())
            if dt<best_dt:
                best_dt = dt
                pose = p

        x = pose[0]
        y = pose[1]
        z = pose[2]
        phi = np.degrees(pose[3])
        theta = np.degrees(pose[4])
        psi = np.degrees(pose[5])

        write(f'update com transform translation {x} {y} {z} rpy {phi} {theta} {psi}\n')

        time.sleep(0.01)
