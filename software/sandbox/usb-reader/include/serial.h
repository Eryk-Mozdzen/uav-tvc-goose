#pragma once

#include <thread>
#include "concurrent_queue.h"

class Serial {
	std::thread thread;
	ConcurrentQueue<uint8_t> queue;

	void reader(const std::string name);

public:
	Serial(const std::string name);
	~Serial();

	void readBytes(uint8_t *dest, const size_t size);
};
