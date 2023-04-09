#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include "comm.h"

class Telnet {
	Comm &communication;

	std::atomic_bool thread_kill;
	std::thread thread_read;
	std::thread thread_write;
	std::mutex mutex;

	void read();
	void write();

public:
	Telnet(const std::string adress, const std::string port, Comm &communication);
	~Telnet();
};
