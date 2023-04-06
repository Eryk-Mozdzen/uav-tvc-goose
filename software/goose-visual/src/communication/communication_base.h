#pragma once

#include <string>
#include "concurrent_queue.h"
#include "transfer.h"

class CommunicationBase {
public:
	using Log = std::pair<Transfer::ID, std::string>;
	using Telemetry = Transfer::FrameRX;
	using Control = Transfer::FrameTX;

protected:
	ConcurrentQueue<Log> &logs;
	ConcurrentQueue<Telemetry> &telemetry;
	ConcurrentQueue<Control> &controls;
	Transfer transfer;

public:
	CommunicationBase(ConcurrentQueue<Log> &logs, ConcurrentQueue<Telemetry> &telemetry, ConcurrentQueue<Control> &controls);
};
