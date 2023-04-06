#pragma once

#include <thread>
#include "transfer.h"
#include "concurrent_queue.h"
#include "communication_base.h"

class Display {
	ConcurrentQueue<CommunicationBase::Log> &logs;
	ConcurrentQueue<CommunicationBase::Telemetry> &telemetry;

	bool thread_kill;
	std::thread logger;
	std::thread viewer;

	void readLog();
	void readTelemetry();

public:
	Display(ConcurrentQueue<CommunicationBase::Log> &logs, ConcurrentQueue<CommunicationBase::Telemetry> &telemetry);
	~Display();
};
