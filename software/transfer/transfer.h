/**
 * @file transfer.h
 * @author Eryk Możdżeń (emozdzen@gmail.com)
 * @brief Implemetation of Transfer class.
 * @date 2023-03-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "frames.h"
#include <cstdint>
#include <cstring>

/**
 * @brief Class used for encoding and decoding data frames
 * @see https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
 * @see https://en.wikipedia.org/wiki/Cyclic_redundancy_check
 */
class Transfer {
private:
	static constexpr size_t max_length = 250;		/*!< Maximal size od payload */
	static constexpr uint8_t start_byte = 0x00;		/*!< Frame start byte */

	uint8_t frame_buffer[max_length + 5];			/*!< Accumulation buffer */
	size_t counter;									/*!< Counter of accumulated bytes */

	/**
	 * @brief Resets accumulation buffer.
	 *
	 */
	void reset();

	/**
	 * @brief Calculate CRC-8 from buffer.
	 *
	 * @param buffer Buffer
	 * @param size Buffer size
	 * @return uint8_t Value of CRC8 from given buffer.
	 */
	static uint8_t calculate_CRC(const uint8_t *buffer, const size_t size);

	/**
	 * @brief Encode (in place) buffer of bytes using COBS.
	 *
	 * @param buffer Buffer
	 * @param size Buffer size
	 * @return uint8_t Value of overhead byte.
	 */
	static uint8_t encode_COBS(uint8_t *buffer, const size_t size);

	/**
	 * @brief Decode (in place) buffer of bytes using COBS.
	 *
	 * @param buffer Buffer
	 * @param size Buffer size
	 * @param cobs Overhead byte
	 */
	static void decode_COBS(uint8_t *buffer, const size_t size, const uint8_t cobs);

public:

	/**
	 * @brief Identification number of data frame.
	 *
	 */
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

	/**
	 * @brief Construct a new Transfer object
	 *
	 */
	Transfer();

	/**
	 * @brief Put one byte of data from stream to accumulation buffer.
	 *
	 * @param byte Data byte
	 */
	void consume(const uint8_t byte);

	/**
	 * @brief Chceck if data frame is complete.
	 *
	 * @param frame Received data frame.
	 * @return true If data frame is complete and received.
	 * @return false If data frame is still not complete.
	 */
	bool receive(FrameRX &frame) const;

	/**
	 * @brief Encode function for raw data.
	 *
	 * @param payload Pointer to first byte.
	 * @param length Number of bytes to be encoded.
	 * @param id ID of message frame.
	 * @return FrameTX
	 */
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

	/**
	 * @brief Encode function for custom class objects.
	 *
	 * @tparam T User custom class.
	 * @param payload Obejct to send.
	 * @param id ID of message frame.
	 * @return FrameTX
	 */
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
