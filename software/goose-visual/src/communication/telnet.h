#pragma once

#include <thread>
#include <mutex>
#include "comm.h"
#include "transfer.h"

class Telnet {
	Comm &communication;

	bool thread_kill;
	std::thread thread_read;
	std::thread thread_write;
	std::mutex mutex;

	void read();
	void write();

public:
	Telnet(const std::string adress, const std::string port, Comm &communication);
	~Telnet();
};
