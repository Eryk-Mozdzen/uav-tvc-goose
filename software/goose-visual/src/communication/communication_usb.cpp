#include "communication_usb.h"
#include <iostream>
#include "escape_codes.h"

CommunicationUSB::CommunicationUSB(const std::string port, ConcurrentQueue<Log> &logs, ConcurrentQueue<Telemetry> &telemetry, ConcurrentQueue<Control> &controls) :
	CommunicationBase{logs, telemetry, controls}, name{port}, thread_kill{false}, thread{&CommunicationUSB::threadRW, this} {

}

CommunicationUSB::~CommunicationUSB() {
	thread_kill = true;
	thread.join();

	if(serial.IsOpen())
		serial.Close();
}

void CommunicationUSB::threadRW() {

	while(!thread_kill) {

		try {
			serial.Open(name);
			serial.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
			serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
			serial.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
			serial.SetParity(LibSerial::Parity::PARITY_NONE);
			serial.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

			while(!thread_kill) {
				LibSerial::DataBuffer buffer;

				try {
					serial.Read(buffer, buffer_size, timeout_ms);
				} catch(const std::exception &e) {
					serial.Close();
					std::cerr << EscapeCode::RED << "Port closed unexpectedly" << std::endl;
					break;
				}

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

		} catch(const std::exception &e) {

		}

	}
}
