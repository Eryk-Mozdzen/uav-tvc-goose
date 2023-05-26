/**
 * @file frames.h
 * @author Eryk Możdżeń (emozdzen@gmail.com)
 * @brief Definition of receive and transmit frames.
 * @date 2023-03-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <cstdint>
#include <cstring>

namespace _transfer {

	/**
	 * @brief Frame used for transmitting data.
	 *
	 * @tparam MAX_LENGTH Maximum number of data bytes.
	 */
	template<size_t MAX_LENGTH>
	struct FrameTX {
		size_t length;
		uint8_t buffer[MAX_LENGTH + 5];
	};

	/**
	 * @brief Frame used for receiving data.
	 *
	 * @tparam ID ID of message frame.
	 * @tparam MAX_LENGTH Maximum number of data bytes.
	 */
	template<typename ID, size_t MAX_LENGTH>
	struct FrameRX {
		ID id;
		size_t length;
		uint8_t payload[MAX_LENGTH];

		/**
		 * @brief Copy payload data to custom class object.
		 *
		 * @tparam T User custom class.
		 * @param dest Destination buffer.
		 * @return true If custom class have the same size as received data.
		 * @return false If copy cannot be done.
		 */
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
