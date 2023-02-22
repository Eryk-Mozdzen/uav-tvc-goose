import re
import serial
import numpy as np

ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200)

J = np.empty([0, 9], dtype=float)
n = 0

while True:
	line = re.sub(' +', ' ', ser.readline().decode('ascii').rstrip()).split(' ')

	if len(line)<4:
		continue

	if line[0]!='MAG':
		continue

	x = float(line[1])
	y = float(line[2])
	z = float(line[3])

	X = [x, y, z]

	n = n + 1
	J = np.vstack([J, [x**2, y**2, z**2, x*y, x*z, y*z, x, y, z]])
	K = np.ones([n, 1], dtype=float)

	if n<9:
		continue

	ABC = np.linalg.inv(J.transpose().dot(J)).dot(J.transpose()).dot(K)

	poly = np.append(ABC, -1)
	
	#print(f'samples: {n: d}')

	Amat=np.array([
		[poly[0],     poly[3]/2.0, poly[4]/2.0, poly[6]/2.0],
		[poly[3]/2.0, poly[1],     poly[5]/2.0, poly[7]/2.0],
		[poly[4]/2.0, poly[5]/2.0, poly[2],     poly[8]/2.0],
		[poly[6]/2.0, poly[7]/2.0, poly[8]/2.0, poly[9]    ]
	])

	A3 = Amat[0:3,0:3]
	A3inv = np.linalg.inv(A3)
	ofs = poly[6:9]/2.0
	center = -np.dot(A3inv, ofs)

	Tofs = np.eye(4, dtype=float)
	Tofs[3,0:3] = center
	R = np.dot(Tofs, np.dot(Amat, Tofs.T))

	R3 = R[0:3,0:3]

	s1 = -R[3, 3]
	R3S = R3/s1
	(el, ec) = np.linalg.eig(R3S)

	G = np.sqrt(1.0/np.abs(el))
	R = np.linalg.inv(ec)

	#print('Center\n', center)
	#print('Axes\n', axes)
	#print('Rotation matrix\n', inve)

	A = np.identity(3, dtype=float)*0.0001
	G = np.diag(G)

	T = R.transpose().dot(A.dot(G)).dot(R)

	#print(T)

	calibrated = T.dot(X - center)

	#print('P', poly)
	#print('X ', X)
	#print('C ', center)
	#print('T\n', T)
	print(f'calib: {calibrated[0]:+15.2f} {calibrated[1]:+15.2f} {calibrated[2]:+15.2f}')
