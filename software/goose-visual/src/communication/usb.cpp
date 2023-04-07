#include "usb.h"
#include <iostream>

USB::USB(const std::string port, Comm &communication) : communication{communication}, name{port}, thread_kill{false},
	thread_read{&USB::read, this},
	thread_write{&USB::write, this} {

	mutex.unlock();
}

USB::~USB() {
	thread_kill = true;

	thread_read.join();
	thread_write.join();

	if(serial.IsOpen())
		serial.Close();
}

void USB::open() {
	mutex.lock();
	if(!serial.IsOpen()) {
		serial.Open(name);
		serial.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
		serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
		serial.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
		serial.SetParity(LibSerial::Parity::PARITY_NONE);
		serial.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
	}
	mutex.unlock();
}

void USB::read() {

	while(!thread_kill) {

		try {
			open();

			Transfer transfer;

			while(!thread_kill) {
				LibSerial::DataBuffer buffer;

				mutex.lock();
				serial.Read(buffer, buffer_size, timeout_ms);
				mutex.unlock();
				//std::cout << "read" << std::endl;

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

		} catch(const LibSerial::ReadTimeout &e) {
			//std::cerr << "timeout " << name << std::endl;
			serial.Close();
			mutex.unlock();
		} catch(const LibSerial::OpenFailed &e) {
			mutex.unlock();
		} catch(const LibSerial::NotOpen &e) {
			mutex.unlock();
		} catch(const std::system_error &e) {

		}

	}
}

void USB::write() {

	while(!thread_kill) {

		try {
			open();

			while(!thread_kill) {
				Comm::Control control;

				if(communication.controls.pop(control)) {
					mutex.lock();
					serial.Write(LibSerial::DataBuffer(control.buffer, control.buffer + control.length));
					serial.FlushOutputBuffer();
					mutex.unlock();
					//std::cout << "send" << std::endl;
				}
			}

		} catch(const LibSerial::OpenFailed &e) {
			mutex.unlock();
		} catch(const LibSerial::NotOpen &e) {
			mutex.unlock();
		}

	}
}
