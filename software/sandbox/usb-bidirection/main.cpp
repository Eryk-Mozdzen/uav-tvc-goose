#include <iostream>
#include <thread>
#include <chrono>
#include "transfer.h"
#include <libserial/SerialPort.h>

void timer(bool *signal, const int period) {

	while(1) {
		*signal = true;

		std::this_thread::sleep_for(std::chrono::milliseconds(period));
	}
}

int main(int argc, char **argv) {

	if(argc!=3) {
		std::cerr << "port not specified or period" << std::endl;
		return 1;
	}

	LibSerial::SerialPort serial;
	Transfer transfer;

	serial.Open(argv[1]);
	serial.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
	serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
	serial.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
	serial.SetParity(LibSerial::Parity::PARITY_NONE);
	serial.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

	int counter = 0;
	const int period = atoi(argv[2]);
	bool signal = false;
	std::thread timer_thread(timer, &signal, period);

	while(true) {

		if(signal) {
			signal = false;
			counter++;

			const Transfer::FrameTX tx = Transfer::encode(2137.f, Transfer::ID::CONTROL_MOTOR_THROTTLE);

			std::cout << "send frame " << counter << std::endl;
			serial.Write(std::vector<uint8_t>(tx.buffer, tx.buffer + tx.length));
		}

		char byte;

		try {
			serial.ReadByte(byte, 10000000);
		} catch(const std::exception &e) {
			std::cout << "connection lost" << std::endl << std::endl;
			serial.Close();
			return 1;
		}

		transfer.consume(byte);

		Transfer::FrameRX rx;
		if(transfer.receive(rx)) {

			if(rx.id>=Transfer::LOG_DEBUG && rx.id<=Transfer::LOG_ERROR)
				std::cout << std::string(reinterpret_cast<char *>(rx.payload), rx.length) << std::endl;
		}
	}
}

