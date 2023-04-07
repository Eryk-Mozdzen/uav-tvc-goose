#include "telnet.h"
#include <iostream>

Telnet::Telnet(const std::string address, const std::string port, Comm &communication) : communication{communication}, thread_kill{false},
	thread_read{&Telnet::read, this},
	thread_write{&Telnet::write, this} {

	(void)address;
	(void)port;

	mutex.unlock();
}

Telnet::~Telnet() {
	thread_kill = true;
	thread_read.join();
	thread_write.join();
}

void Telnet::read() {

	while(!thread_kill) {

	}
}

void Telnet::write() {

	while(!thread_kill) {

	}
}
