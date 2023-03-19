#include "serial.h"
#include <cassert>
#include <iostream>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

Serial::Serial(const std::string name) : thread{&Serial::reader, this, name} {

}

Serial::~Serial() {
	thread.join();
}

void Serial::reader(const std::string name) {
	int port = open(name.c_str(), O_RDWR | O_NOCTTY);
	
	assert(port);

	while(1) {
		char c;
		read(port, &c, 1);

		//std::cout << (int)c << " ";
		//std::cout.flush();

		queue.push(c);
	}

	close(port);
}

void Serial::readBytes(uint8_t *dest, const size_t size) {

	for(size_t i=0; i<size; i++) {
		dest[i] = queue.pop();
	}
}
