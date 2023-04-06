#pragma once

#include <cstdint>
#include <cstring>

namespace _transfer {

	template<typename ID, size_t MAX_LENGTH>
	struct FrameRX {
		ID id;
		size_t length;
		uint8_t payload[MAX_LENGTH];

		template<typename T>
		bool getPayload(T &dest) const {
			if(sizeof(T)!=length) {
				return false;
			}

			memcpy(&dest, payload, sizeof(T));

			return true;
		}
	};

}
