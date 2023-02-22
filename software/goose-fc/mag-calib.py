import serial
import re
import math
import cv2
import numpy as np

class Calibration:
	def __init__(self, min_dist):
		self.samples = []
		self.__min_dist = min_dist

		self.__offset = [0, 0, 0]
		self.__scale = [1, 1, 1]

	def __is_far_enough(self, mag):
		for sample in self.samples:
			diff = [
				mag[0] - sample[0],
				mag[1] - sample[1],
				mag[2] - sample[2],
			]
			dist = math.sqrt(diff[0]**2 + diff[1]**2 + diff[2]**2)

			if dist < self.__min_dist:
				return False
		
		return True

	def __get_mean(self, dataset):
		sum = 0

		for element in dataset:
			sum +=element

		return sum/len(dataset)

	def add_sample(self, mag):
		if not self.__is_far_enough(mag):
			return False

		self.samples.append(mag)

		if len(self.samples)<2:
			return False

		self.__offset = [
			self.__get_mean([sample[0] for sample in self.samples]),
			self.__get_mean([sample[1] for sample in self.samples]),
			self.__get_mean([sample[2] for sample in self.samples])
		]

		magic_number = 8.57

		self.__scale = [
			self.__get_mean([np.abs(sample[0] - self.__offset[0]) for sample in self.samples])*magic_number,
			self.__get_mean([np.abs(sample[1] - self.__offset[1]) for sample in self.samples])*magic_number,
			self.__get_mean([np.abs(sample[2] - self.__offset[2]) for sample in self.samples])*magic_number
		]

		return True

	def get_params(self):
		return self.__offset, self.__scale

	def get_calibrated(self, mag):
		offset, scale = self.get_params()

		return [
			(mag[0] - offset[0])*scale[0],
			(mag[1] - offset[1])*scale[1],
			(mag[2] - offset[2])*scale[2],
		]

class Viewer:
	def __init__(self, name, scale, width=640, height=420):
		self.name = name
		self.width = width
		self.height = height

		self.backgroud = np.zeros(shape=(height, width, 3), dtype=np.uint8)
		self.img = np.copy(self.backgroud)

		self.scale = scale

	def draw_point(self, point):
		center = [int(self.width/2), int(self.height/2)]

		if math.isnan(point[0]) or math.isnan(point[1]) or math.isnan(point[2]):
			return

		point_xy = (
			int(point[0]*self.scale + center[0]),
			int(point[1]*self.scale + center[1])
		)
		
		point_yz = (
			int(point[1]*self.scale + center[0]),
			int(point[2]*self.scale + center[1])
		)

		point_zx = (
			int(point[2]*self.scale + center[0]),
			int(point[0]*self.scale + center[1])
		)

		cv2.circle(self.img, point_xy, 3, (255, 0, 0), -1)
		cv2.circle(self.img, point_yz, 3, (0, 255, 0), -1)
		cv2.circle(self.img, point_zx, 3, (0, 0, 255), -1)
		cv2.circle(self.img, center, self.scale, (255, 255, 255), 1)

	def render(self, clear):
		cv2.imshow(self.name, self.img)

		if clear:
			self.img = np.copy(self.backgroud)

with serial.Serial(port='/dev/ttyACM0', baudrate=115200) as ser:

	calibration = Calibration(min_dist=0.1)
	viewer_raw   = Viewer(name="mag raw",		 scale=250)
	viewer_calib = Viewer(name="mag calibrated", scale=125)

	while True:
		line = re.sub(' +', ' ', ser.readline().decode('ascii').rstrip()).split(' ')

		if len(line)!=4:
			continue

		mag = [
			float(line[1]),
			float(line[2]),
			float(line[3])
		]

		if cv2.waitKey(1) == ord('x'):
			break

		if not calibration.add_sample(mag):
			continue

		viewer_raw.draw_point(mag)
		viewer_raw.render(clear=False)

		mean_len = 0

		for sample in calibration.samples:
			calibrated = calibration.get_calibrated(sample)
			
			mean_len +=math.sqrt(calibrated[0]**2 + calibrated[1]**2 + calibrated[2]**2)
			
			viewer_calib.draw_point(calibrated)
		
		mean_len /=len(calibration.samples)
		
		viewer_calib.render(clear=True)
		
		offset, scale = calibration.get_params()

		print(line[1:])
		print(f'offset: {offset[0] : 10.5f} {offset[1] : 10.5f} {offset[2] : 10.5f}')
		print(f'scale:  {scale[0] : 10.5f} {scale[1] : 10.5f} {scale[2] : 10.5f}')
		print(f'len:    {mean_len: 10.5f}')
