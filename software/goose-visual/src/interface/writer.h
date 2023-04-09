#pragma once

#include <thread>
#include "transfer.h"
#include "comm.h"

class Writer {
	Comm &communication;

	bool thread_kill;
	std::thread thread;

	void write();

public:
	Writer(Comm &communication);
	~Writer();
};
