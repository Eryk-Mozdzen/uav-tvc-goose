#pragma once

#include <thread>
#include "transfer.h"
#include "comm.h"

class Writer {
	ConcurrentQueue<Comm::Control> &controls;

	bool thread_kill;
	std::thread thread;

	void write();

public:
	Writer(ConcurrentQueue<Comm::Control> &controls);
	~Writer();
};
