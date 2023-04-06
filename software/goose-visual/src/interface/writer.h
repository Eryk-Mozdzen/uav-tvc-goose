#pragma once

#include <thread>
#include "transfer.h"
#include "concurrent_queue.h"
#include "communication_base.h"

class Writer {
	ConcurrentQueue<CommunicationBase::Control> &controls;

	bool thread_kill;
	std::thread thread;

	void write();

public:
	Writer(ConcurrentQueue<CommunicationBase::Control> &controls);
	~Writer();
};
