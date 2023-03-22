#include "transfer.h"
#include <iostream>
#include <iomanip>
#include <libserial/SerialStream.h>

int main(int argc, char **argv) {
	(void)argc;

	LibSerial::SerialStream serial(argv[1],
		LibSerial::BaudRate::BAUD_115200,
		LibSerial::CharacterSize::CHAR_SIZE_8,
		LibSerial::FlowControl::FLOW_CONTROL_NONE,
		LibSerial::Parity::PARITY_NONE,
		LibSerial::StopBits::STOP_BITS_1
	);

	Transfer transfer;

	const std::ios_base::fmtflags default_flags = std::cout.flags();

	while(true) {
        char byte;
        serial.get(byte);

		transfer.consume(byte);

		Transfer::FrameRX rx;
		if(transfer.receive(rx)) {
			
			std::cout.flags(default_flags);
			std::cout << "[id: " << static_cast<int>(rx.id) << ", len: " << rx.length << "] ";

			switch(rx.id) {
				case Transfer::LOG_DEBUG:
				case Transfer::LOG_INFO:
				case Transfer::LOG_WARNING:
				case Transfer::LOG_ERROR: {
					std::cout << std::string(reinterpret_cast<char *>(rx.payload), rx.length) << std::endl;
				} break;

				case Transfer::TELEMETRY_ESTIMATION_ATTITUDE: {
					struct Quaternion {float w, i, j, k;} q;
					rx.getPayload(q);

					std::cout << std::showpos << std::fixed << std::setprecision(5);
					std::cout << q.w << " " << q.i << " " << q.j << " " << q.k << std::endl;
				} break;

				default: break;
			}
		}
    }
}
