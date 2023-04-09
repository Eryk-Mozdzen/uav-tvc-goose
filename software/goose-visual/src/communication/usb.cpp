#include "usb.h"
#include <libserial/SerialPort.h>
#include "transfer.h"

USB::USB(const std::string port, Comm &communication) : communication{communication}, name{port},
	thread_kill{false},
	thread_read{&USB::read, this},
	thread_write{&USB::write, this} {

}

USB::~USB() {
	thread_kill = true;

	thread_read.join();
	thread_write.join();
}

void USB::read() {

	LibSerial::SerialPort serial;

	serial.Open(name, std::ios_base::in);
	serial.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
	serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
	serial.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
	serial.SetParity(LibSerial::Parity::PARITY_NONE);
	serial.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

	Transfer transfer;

	while(!thread_kill) {
		LibSerial::DataBuffer buffer;

		serial.Read(buffer, buffer_size, timeout_ms);

		for(const uint8_t byte : buffer) {
			transfer.consume(byte);

			Transfer::FrameRX frame;

			if(transfer.receive(frame)) {
				if(frame.id>=Transfer::ID::LOG_DEBUG && frame.id<=Transfer::ID::LOG_ERROR) {
					communication.logs.push(std::make_pair(frame.id, std::string(reinterpret_cast<char *>(frame.payload), frame.length)));
				} else {
					communication.telemetry.push(frame);
				}
			}
		}
	}

	serial.Close();
}

void USB::write() {

	LibSerial::SerialPort serial;

	serial.Open(name, std::ios_base::out);
	serial.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
	serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
	serial.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
	serial.SetParity(LibSerial::Parity::PARITY_NONE);
	serial.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

	while(!thread_kill) {
		Comm::Control control;

		if(communication.controls.pop(control)) {
			serial.Write(LibSerial::DataBuffer(control.buffer, control.buffer + control.length));
			serial.DrainWriteBuffer();
		}
	}

	serial.Close();
}
