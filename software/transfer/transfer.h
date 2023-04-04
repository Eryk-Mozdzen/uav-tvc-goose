#pragma once

#include "frame_tx.h"
#include "frame_rx.h"
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
		LOG_DEBUG = 0,
		LOG_INFO = 1,
		LOG_WARNING = 2,
		LOG_ERROR = 3,

		TELEMETRY_SENSOR_ACCELERATION = 10,
		TELEMETRY_SENSOR_GYRATION = 11,
		TELEMETRY_SENSOR_MAGNETIC_FIELD = 12,
		TELEMETRY_SENSOR_PRESSURE = 13,
		TELEMETRY_SENSOR_VOLTAGE = 14,
		TELEMETRY_SENSOR_CURRENT = 15,

		TELEMETRY_ESTIMATION_ATTITUDE = 20,
		TELEMETRY_ESTIMATION_LINEAR_ACCELERATION = 21,
		TELEMETRY_ESTIMATION_ALTITUDE = 22,
		TELEMETRY_ESTIMATION_BATTERY_LEVEL = 23,

		TELEMETRY_INPUTS_SERVOS = 30,
		TELEMETRY_INPUTS_MOTOR = 31,

		CONTROL_ATTITUDE_SETPOINT = 40,
		CONTROL_MOTOR_THROTTLE = 41
	};

	using FrameTX = _transfer::FrameTX<max_length>;
	using FrameRX = _transfer::FrameRX<ID, max_length>;

	Transfer();

	void consume(const uint8_t byte);
	bool receive(FrameRX &frame) const;

	static FrameTX encode(const void *payload, const size_t length, const ID id) {
		FrameTX frame;
	
		memcpy(&frame.buffer[5], payload, length);
		frame.buffer[4] = id;
		frame.buffer[3] = length;
		frame.buffer[2] = calculate_CRC(&frame.buffer[4], 1 + length);
		frame.buffer[1] = encode_COBS(&frame.buffer[2], 3 + length);
		frame.buffer[0] = start_byte;

		frame.length = length + 5;

		return frame;
	}

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
