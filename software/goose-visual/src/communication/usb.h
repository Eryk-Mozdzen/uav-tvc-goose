#pragma once

#include <thread>
#include <atomic>
#include "comm.h"

class USB {
	static constexpr int buffer_size = 64;
	static constexpr int timeout_ms = 100;

	Comm &communication;

	const std::string name;

	std::atomic_bool thread_kill;
	std::thread thread_read;
	std::thread thread_write;

	void read();
	void write();

public:
	USB(const std::string port, Comm &communication);
	~USB();
};
