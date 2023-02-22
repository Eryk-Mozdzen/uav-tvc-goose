#pragma once

#include <cstdint>

template<uint32_t ADDRESS, int SIZE>
class DeviceRegister {
	public:
		static constexpr uint32_t address = ADDRESS;
		static constexpr int size = SIZE;
};
