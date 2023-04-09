#pragma once

#include <thread>
#include <mutex>
#include "transfer.h"
#include "comm.h"

class Printer {
	Comm &commuication;

	std::mutex mutex;
	bool thread_kill;
	std::thread logger;
	std::thread viewer;

	void readLog();
	void readTelemetry();

public:
	Printer(Comm &commuication);
	~Printer();
};
