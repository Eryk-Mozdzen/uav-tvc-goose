#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <libserial/SerialPort.h>
#include "communication_base.h"

class CommunicationUSB : public CommunicationBase {
	static constexpr int buffer_size = 64;
	static constexpr int timeout_ms = 100;

	const std::string name;
	LibSerial::SerialPort serial;
	bool thread_kill;
	std::thread thread_read;
	std::thread thread_write;
	std::mutex mutex;

	void open();
	void read();
	void write();

public:
	CommunicationUSB(const std::string port, ConcurrentQueue<Log> &logs, ConcurrentQueue<Telemetry> &telemetry, ConcurrentQueue<Control> &controls);
	~CommunicationUSB();
};
