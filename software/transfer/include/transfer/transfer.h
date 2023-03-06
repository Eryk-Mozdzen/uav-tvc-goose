#pragma once

#include <transfer/frame_tx.h>
#include <transfer/frame_rx.h>
#include <cstdint>
#include <cstring>

class Transfer {
	static constexpr size_t max_length = 250;
	static constexpr uint8_t start_byte = 0x00;

	uint8_t frame_buffer[max_length + 5];
	size_t counter;

	void reset();

	static uint8_t calculate_CRC(const uint8_t *buffer, const size_t size);
	static uint8_t encode_COBS(uint8_t *buffer, const size_t size);
	static void decode_COBS(uint8_t *buffer, const size_t size, const uint8_t cobs);

public:

	enum ID {
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR,

		TELEMETRY_SENSOR_ACCELERATION,
		TELEMETRY_SENSOR_GYRATION,
		TELEMETRY_SENSOR_MAGNETIC_FIELD,
		TELEMETRY_SENSOR_PRESSURE,
		TELEMETRY_SENSOR_VOLTAGE,
		TELEMETRY_SENSOR_CURRENT,

		TELEMETRY_ESTIMATION_ATTITUDE,
		TELEMETRY_ESTIMATION_LINEAR_ACCELERATION,
		TELEMETRY_ESTIMATION_ALTITUDE,
		TELEMETRY_ESTIMATION_BATTERY_LEVEL
	};

	using FrameTX = _transfer::FrameTX<max_length>;
	using FrameRX = _transfer::FrameRX<ID, max_length>;

	Transfer();

	void consume(const uint8_t byte);
	bool receive(FrameRX &frame) const;

	template<typename T>
	static FrameTX encode(const T &payload, const ID id) {
		static_assert(sizeof(T)<=max_length, "payload too big to encode");

		FrameTX frame;
	
		memcpy(&frame.buffer[5], &payload, sizeof(T));
		frame.buffer[4] = id;
		frame.buffer[3] = sizeof(T);
		frame.buffer[2] = calculate_CRC(&frame.buffer[4], 1 + sizeof(T));
		frame.buffer[1] = encode_COBS(&frame.buffer[2], 3 + sizeof(T));
		frame.buffer[0] = start_byte;

		frame.length = sizeof(T) + 5;

		return frame;
	}
};
