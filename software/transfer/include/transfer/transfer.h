#pragma once

#include <cstdint>
#include <cstring>

class Transfer {
	static constexpr size_t max_length = 250;
	static constexpr uint8_t start_byte = 0x00;

	uint8_t frame[max_length + 5];
	size_t counter;

	void reset();

	static uint8_t calculate_CRC(const uint8_t *buffer, const size_t size);
	static uint8_t encode_COBS(uint8_t *buffer, const size_t size);
	static void decode_COBS(uint8_t *buffer, const size_t size, const uint8_t cobs);

public:

	Transfer();

	void consume(const uint8_t byte);
	bool receive(uint8_t &id, uint8_t *buffer, size_t &length) const;

	template<typename T>
	static size_t encode(uint8_t *buffer, const T &payload, const uint8_t id) {
		static_assert(sizeof(T)<=max_length);

		memcpy(buffer + 5, &payload, sizeof(T));
		buffer[4] = id;
		buffer[3] = sizeof(T);
		buffer[2] = calculate_CRC(&buffer[4], 1 + sizeof(T));
		buffer[1] = encode_COBS(&buffer[2], 3 + sizeof(T));
		buffer[0] = start_byte;

		return sizeof(T) + 5;
	}
};
