#include "transfer.h"
#include <iostream>
#include <iomanip>
#include <libserial/SerialPort.h>
#include "escape_codes.h"

int main(int argc, char **argv) {
	(void)argc;

	LibSerial::SerialPort serial;
	Transfer transfer;

	const std::ios_base::fmtflags default_flags = std::cout.flags();

	while(true) {
		
		try {
			serial.Open(argv[1]);
			serial.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
			serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
			serial.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
			serial.SetParity(LibSerial::Parity::PARITY_NONE);
			serial.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

			while(true) {
				char byte;

				try {
					serial.ReadByte(byte, 100);
				} catch(const std::exception &e) {
					std::cout.flags(default_flags);
					std::cout << std::endl << EscapeCode::RED << "connection lost" << std::endl << std::endl;
					serial.Close();
					break;
				}

				transfer.consume(byte);

				Transfer::FrameRX rx;
				if(transfer.receive(rx)) {
					
					std::cout.flags(default_flags);

					switch(rx.id) {
						case Transfer::LOG_DEBUG: {
							std::cout << EscapeCode::GRAY << std::string(reinterpret_cast<char *>(rx.payload), rx.length) << std::endl;
						} break;
						case Transfer::LOG_INFO: {
							std::cout << EscapeCode::CYAN << std::string(reinterpret_cast<char *>(rx.payload), rx.length) << std::endl;
						} break;
						case Transfer::LOG_WARNING: {
							std::cout << EscapeCode::YELLOW << std::string(reinterpret_cast<char *>(rx.payload), rx.length) << std::endl;
						} break;
						case Transfer::LOG_ERROR: {
							std::cout << EscapeCode::RED << std::string(reinterpret_cast<char *>(rx.payload), rx.length) << std::endl;
						} break;

						case Transfer::TELEMETRY_ESTIMATION_ATTITUDE: {
							struct Quaternion {float w, i, j, k;} q;
							rx.getPayload(q);

							std::cout << EscapeCode::GRAY << std::showpos << std::fixed << std::setprecision(5);
							std::cout << q.w << " " << q.i << " " << q.j << " " << q.k << std::endl;
						} break;

						default: break;
					}
				}
			}

		} catch(const std::exception &e) {

		}

	}
}
