import serial
import re
import math
import cv2
import numpy as np

def draw_point3d(img, point, scale):
	center = [320, 240]

	point_xy = (
		int(point[0]*scale + center[0]),
		int(point[1]*scale + center[1])
	)
	
	point_yz = (
		int(point[1]*scale + center[0]),
		int(point[2]*scale + center[1])
	)

	point_zx = (
		int(point[2]*scale + center[0]),
		int(point[0]*scale + center[1])
	)

	cv2.circle(img, center, 5, (255, 255, 255), -1)
	cv2.circle(img, point_xy, 1, (255, 0, 0), -1)
	cv2.circle(img, point_yz, 1, (0, 255, 0), -1)
	cv2.circle(img, point_zx, 1, (0, 0, 255), -1)

def show_calibrated(samples, scale, offset):
	img = np.zeros(shape=(480, 640, 3), dtype=np.uint8)

	mean_len = 0
	for sample in samples:

		calibrated = [
			(sample[0] - offset[0])/scale[0],
			(sample[1] - offset[1])/scale[1],
			(sample[2] - offset[2])/scale[2],
		]

		mean_len +=math.sqrt(calibrated[0]**2 + calibrated[1]**2 + calibrated[2]**2)

		draw_point3d(img, calibrated, 150)

	cv2.imshow("mag calibrated", img)
	print(f'mean calibrated len: {mean_len/len(samples): 10.5f}')

with serial.Serial(port='/dev/ttyACM0', baudrate=115200) as ser:

	samples = []
	background = np.zeros(shape=(480, 640, 3), dtype=np.uint8)

	while True:
		line = re.sub(' +', ' ', ser.readline().decode('ascii').rstrip()).split(' ')

		if len(line)!=4:
			continue

		mag = [
			float(line[1]),
			float(line[2]),
			float(line[3])
		]

		collect = True

		for sample in samples:
			diff = [
				mag[0] - sample[0],
				mag[1] - sample[1],
				mag[2] - sample[2],
			]
			dist = math.sqrt(diff[0]**2 + diff[1]**2 + diff[2]**2)

			if dist < 50:
				collect = False
				break

		if cv2.waitKey(1) == ord('x'):
			break

		if not collect:
			continue

		samples.append(mag)

		print(line)

		draw_point3d(background, mag, 0.25)

		cv2.imshow("mag raw", background)

		if len(samples)<2:
			continue

		mean = [0, 0, 0]
		for sample in samples:
			mean[0] +=sample[0]
			mean[1] +=sample[1]
			mean[2] +=sample[2]
		mean[0] /=len(samples)
		mean[1] /=len(samples)
		mean[2] /=len(samples)

		print(f'mean:  {mean[0] : 10.5f} {mean[1] : 10.5f} {mean[2] : 10.5f}')

		#max = [0, 0, 0]
		#for sample in samples:
		#	max[0] = np.maximum(max[0], np.abs(sample[0]))
		#	max[1] = np.maximum(max[1], np.abs(sample[1]))
		#	max[2] = np.maximum(max[2], np.abs(sample[2]))
		#scale = [
		#	1/max[0],
		#	1/max[1],
		#	1/max[2]
		#]

		mean_abs = [0, 0, 0]
		for sample in samples:
			mean_abs[0] +=np.abs(sample[0] - mean[0])
			mean_abs[1] +=np.abs(sample[1] - mean[1])
			mean_abs[2] +=np.abs(sample[2] - mean[2])
		mean_abs[0] /=len(samples)
		mean_abs[1] /=len(samples)
		mean_abs[2] /=len(samples)

		frac = ((3*math.pi**2)/(8*(3*math.pi - 4)))**1.7
		#frac = 1

		scale = [
			mean_abs[0]/frac,
			mean_abs[1]/frac,
			mean_abs[2]/frac,
		]

		print(f'scale: {scale[0] : 10.5f} {scale[1] : 10.5f} {scale[2] : 10.5f}')

		show_calibrated(samples, scale, mean)