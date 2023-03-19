#include "transfer.h"
#include <iostream>
#include "serial.h"

int main(int argc, char **argv) {
	(void)argc;

	Serial serial(argv[1]);

	Transfer transfer;

	while(1) {

		uint8_t buffer[16];
		serial.readBytes(buffer, sizeof(buffer));

		for(size_t i=0; i<sizeof(buffer); i++) {
			const uint8_t byte = buffer[i];

			transfer.consume(byte);

			Transfer::FrameRX rx;
			if(transfer.receive(rx)) {

				std::cout << "[id: " << static_cast<int>(rx.id) << ", len: " << rx.length << "] ";

				std::string str;
				for(size_t i=0; i<rx.length; i++)
					str.push_back(static_cast<char>(rx.payload[i]));

				std::cout << str << std::endl;
			}
		}
		
	}	
}
