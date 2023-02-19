import math, cv2, time
import numpy as np
import serial
import re

def rot_x(angle):
	mat = [
		[1, 0,  0],
		[0, math.cos(angle), -math.sin(angle)],
		[0, math.sin(angle),  math.cos(angle)]
	]
	return mat

def rot_y(angle):
	mat = [
		[ math.cos(angle), 0, math.sin(angle)],
		[0,	1,  0],
		[-math.sin(angle), 0, math.cos(angle)]
	]
	return mat

def rot_z(angle):
	mat = [
		[math.cos(angle), -math.sin(angle), 0],
		[math.sin(angle),  math.cos(angle), 0],
		[0, 0,  1],
	]
	return mat

def projection(point):
	view_plane_dist = 400
	view_center_dist = 100

	s = view_plane_dist/(view_center_dist + point[0][0])
	
	x = int(point[1][0]*s) + 320
	y = int(point[2][0]*s) + 240
	return (x, y)

def draw_line(img, p1, p2, color):
	a = projection(p1)
	b = projection(p2)
	cv2.line(img, a, b, color, 2)

points = [
	[[-50], [-25], [+5]],
	[[+50], [-25], [+5]],
	[[+50], [+25], [+5]],
	[[-50], [+25], [+5]],
	[[-50], [-25], [-5]],
	[[+50], [-25], [-5]],
	[[+50], [+25], [-5]],
	[[-50], [+25], [-5]]
]

background = np.zeros(shape=(480, 640, 3), dtype=np.uint8)

with serial.Serial(port='/dev/ttyACM0', baudrate=115200) as ser:

	while True:
		line = re.sub(' +', ' ', ser.readline().decode('ascii').rstrip()).split(' ')

		if len(line)!=4:
			continue

		roll  = -float(line[1])*math.pi/180
		pitch = -float(line[2])*math.pi/180
		yaw   = -float(line[3])*math.pi/180 + math.pi/2

		rotation_matrix = np.matmul(np.matmul(rot_z(yaw), rot_y(pitch)), rot_x(roll))

		rotated = [np.matmul(rotation_matrix, point) for point in points]

		img = np.copy(background)

		for m in range(4):
			draw_line(img, rotated[m  ], rotated[(m+1)%4  ], (0, 0, 255))
			draw_line(img, rotated[m+4], rotated[(m+1)%4+4], (255, 0, 0))
			draw_line(img, rotated[m  ], rotated[ m+4     ], (0, 255, 0))

		cv2.putText(img, f'roll:  {roll *180/math.pi: 3.0f} deg', (10, 20), cv2.FONT_HERSHEY_PLAIN, 1, 255)
		cv2.putText(img, f'pitch:{pitch*180/math.pi: 3.0f} deg', (10, 40), cv2.FONT_HERSHEY_PLAIN, 1, 255)
		cv2.putText(img, f'yaw:  {yaw  *180/math.pi: 3.0f} deg', (10, 60), cv2.FONT_HERSHEY_PLAIN, 1, 255)

		cv2.imshow("rotation cube", img)

		if cv2.waitKey(1) == ord('x'):
			break
