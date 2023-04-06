#include "communication_usb.h"
#include <iostream>

CommunicationUSB::CommunicationUSB(const std::string port, ConcurrentQueue<Log> &logs, ConcurrentQueue<Telemetry> &telemetry, ConcurrentQueue<Control> &controls) :
	CommunicationBase{logs, telemetry, controls}, name{port}, thread_kill{false},
	thread_read{&CommunicationUSB::read, this},
	thread_write{&CommunicationUSB::write, this} {

	mutex.unlock();
}

CommunicationUSB::~CommunicationUSB() {
	thread_kill = true;

	thread_read.join();
	thread_write.join();

	if(serial.IsOpen())
		serial.Close();
}

void CommunicationUSB::open() {
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

void CommunicationUSB::read() {

	while(!thread_kill) {

		try {
			open();

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
							logs.push(std::make_pair(frame.id, std::string(reinterpret_cast<char *>(frame.payload), frame.length)));
						} else {
							telemetry.push(frame);
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

void CommunicationUSB::write() {

	while(!thread_kill) {

		try {
			open();

			while(!thread_kill) {
				Control control;

				if(controls.pop(control)) {
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
