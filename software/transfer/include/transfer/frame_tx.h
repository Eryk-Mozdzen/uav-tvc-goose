#pragma once

#include <cstdint>
#include <cstring>

namespace _transfer {

	template<size_t MAX_LENGTH>
	struct FrameTX {
		size_t length;
		uint8_t buffer[MAX_LENGTH + 5];
	};

}
