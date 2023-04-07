#pragma once

#include <thread>
#include <mutex>
#include "transfer.h"
#include "comm.h"

class Printer {
	ConcurrentQueue<Comm::Log> &logs;
	ConcurrentQueue<Comm::Telemetry> &telemetry;

	std::mutex mutex;
	bool thread_kill;
	std::thread logger;
	std::thread viewer;

	void readLog();
	void readTelemetry();

public:
	Printer(ConcurrentQueue<Comm::Log> &logs, ConcurrentQueue<Comm::Telemetry> &telemetry);
	~Printer();
};
