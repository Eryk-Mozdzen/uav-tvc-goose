import re
import serial
import numpy as np

ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200)

n = 1
mag_offset = [0, 0, 0]
mag_mean = [1, 1, 1]

while True:
	line = re.sub(' +', ' ', ser.readline().decode('ascii').rstrip()).split(' ')

	if len(line)<4:
		continue

	if line[0]!='MAG':
		continue

	x = float(line[1])
	y = float(line[2])
	z = float(line[3])
	
	mag_mean[0] = (mag_mean[0]*(n-1) + abs(x-mag_offset[0]))/n
	mag_mean[1] = (mag_mean[1]*(n-1) + abs(y-mag_offset[1]))/n
	mag_mean[2] = (mag_mean[2]*(n-1) + abs(z-mag_offset[2]))/n
	
	mag_offset[0] = (mag_offset[0]*(n-1) + x)/n
	mag_offset[1] = (mag_offset[1]*(n-1) + y)/n
	mag_offset[2] = (mag_offset[2]*(n-1) + z)/n

	mag_mean_len = np.sqrt(mag_mean[0]**2 + mag_mean[1]**2 + mag_mean[2]**2)
	mag_scale = [1, 1, 1]
	mag_scale[0] = 1/(mag_mean[0])
	mag_scale[1] = 1/(mag_mean[1])
	mag_scale[2] = 1/(mag_mean[2])
	
	n = n + 1

	print(f'samples: {n: d}')
	print(f'mag_mean_len: {mag_mean_len: .5f}')
	print(f'offset: [{mag_offset[0]:+6.2f} {mag_offset[1]:+6.2f} {mag_offset[2]:+6.2f}]')
	print(f'scale: [{mag_scale[0]:+6.9f} {mag_scale[1]:+6.9f} {mag_scale[2]:+6.9f}]')
