import math, cv2, time
import numpy as np
import serial
import re

def quaternion_to_rotation(w, i, j, k):
	s = 1/(w*w + i*i + j*j + k*k)

	mat = [
		[1 - 2*s*(j*j + k*k), 2*s*(i*j - k*w), 	  2*s*(i*k + j*w)],
		[2*s*(i*j + k*w),	 1 - 2*s*(i*i + k*k), 2*s*(j*k - i*w)],
		[2*s*(i*k - j*w),	 2*s*(j*k + i*w),	  1 - 2*s*(i*i + j*j)]
	]

	return mat

def projection(point):
	view_plane_dist = 400
	view_center_dist = 100

	s = view_plane_dist/(view_center_dist - point[1][0])
	
	x = int(point[0][0]*s) + 320
	y = int(point[2][0]*s) + 240
	return (x, y)

def draw_line(img, p1, p2, color):
	a = projection(p1)
	b = projection(p2)
	cv2.line(img, a, b, color, 2)

print_one = True

points = [
	[[-25], [-50], [+5]],
	[[+25], [-50], [+5]],
	[[+25], [+50], [+5]],
	[[-25], [+50], [+5]],
	[[-25], [-50], [-5]],
	[[+25], [-50], [-5]],
	[[+25], [+50], [-5]],
	[[-25], [+50], [-5]]
]

background = np.zeros(shape=(480, 640, 3), dtype=np.uint8)

while True:

	try:

		with serial.Serial(port='/dev/ttyACM0', baudrate=115200) as ser:

			print_one = True

			while True:
				line = ser.readline().decode('ascii').rstrip()
				parsed = re.sub(' +', ' ', line.rstrip()).split(' ')

				if len(parsed)!=5:
					print(line)
					continue

				if not 'quat' in parsed[0]:
					print(line)
					continue

				w = float(parsed[1])
				i = float(parsed[2])
				j = float(parsed[3])
				k = float(parsed[4])

				rotation_matrix = quaternion_to_rotation(w, i, j, k)

				rotated = [np.matmul(rotation_matrix, point) for point in points]

				img = np.copy(background)

				for m in range(4):
					draw_line(img, rotated[m  ], rotated[(m+1)%4  ], (0, 0, 255))
					draw_line(img, rotated[m+4], rotated[(m+1)%4+4], (255, 0, 0))
					draw_line(img, rotated[m  ], rotated[ m+4     ], (0, 255, 0))

				cv2.imshow("rotation cube", img)

				if cv2.waitKey(1) == ord('x'):
					quit()
	
	except serial.serialutil.SerialException:
		if print_one:
			print_one = False
			print('\x1b[38;5;9m')
			print('connection lost')

	except KeyboardInterrupt:
		quit()
