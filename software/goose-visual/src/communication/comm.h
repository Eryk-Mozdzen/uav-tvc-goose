#pragma once

#include <string>
#include "concurrent_queue.h"
#include "transfer.h"

struct Comm {
	using Log = std::pair<Transfer::ID, std::string>;
	using Telemetry = Transfer::FrameRX;
	using Control = Transfer::FrameTX;

	ConcurrentQueue<Log> logs;
	ConcurrentQueue<Telemetry> telemetry;
	ConcurrentQueue<Control> controls;
};
