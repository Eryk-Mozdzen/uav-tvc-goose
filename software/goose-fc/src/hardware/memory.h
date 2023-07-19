#pragma once

#include "comm.h"
#include "MutexCPP.h"

class Memory {
	static constexpr uint32_t sector_address = 0x08060000;
	Mutex lock;
	comm::Memory copy;

	Memory();

public:
    Memory(Memory &) = delete;
	void operator=(const Memory &) = delete;

	void synchronize();

	void write(const comm::Memory &data);
	comm::Memory read();
	comm::Memory get() const;

    static Memory & getInstance();
};