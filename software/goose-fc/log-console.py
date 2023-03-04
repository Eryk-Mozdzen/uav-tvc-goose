import serial

print_one = True

while True:

	try:
		with serial.Serial(port='/dev/ttyACM0', baudrate=115200) as ser:
			
			print_one = True

			while True:
				line = ser.readline().decode('ascii').rstrip()
				print(line)

	except UnicodeDecodeError:
		print('\x1b[38;5;9m')
		print('codec error')
			
	except serial.serialutil.SerialException:
		if print_one:
			print_one = False
			print('\x1b[38;5;9m')
			print('connection lost')

	except KeyboardInterrupt:
		quit()