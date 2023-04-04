#pragma once

#include "QueueCPP.h"
#include "MutexCPP.h"

class CommunicationBus {
	Mutex lock_tx;
	Mutex lock_rx;

	CommunicationBus();

public:
	CommunicationBus(CommunicationBus &) = delete;
	void operator=(const CommunicationBus &) = delete;

	void init();
	void write(const uint8_t *src, const uint32_t len);
	void read(uint8_t *dest, const uint32_t len);

	static CommunicationBus& getInstance();
};
