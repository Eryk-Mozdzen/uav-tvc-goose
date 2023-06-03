#include "transfer.h"

Transfer::Transfer() {
	reset();
}

void Transfer::reset() {
	counter = 0;

	memset(frame_buffer, 0xFF, sizeof(frame_buffer));
}

void Transfer::consume(const uint8_t byte) {
	if(counter==0 && byte!=start_byte) {
		return;
	}

	if(counter>0 && byte==start_byte) {
		reset();
	}

	if(counter>=max_length+5) {
		reset();
	}

	frame_buffer[counter] = byte;
	counter++;
}

bool Transfer::receive(FrameRX &frame) const {
	if(counter<5) {
		return false;
	}

	if(frame_buffer[0]!=start_byte) {
		return false;
	}

	uint8_t copy[max_length + 5];
	memcpy(copy, frame_buffer, counter);

	const uint8_t cobs = copy[1];

	decode_COBS(&copy[2], counter - 2, cobs);

	frame.length = copy[3];
	const size_t length_real = counter - 5;

	if(frame.length!=length_real) {
		return false;
	}

	const uint8_t crc = copy[2];
	const uint8_t crc_real = calculate_CRC(&copy[4], frame.length + 1);

	if(crc!=crc_real) {
		return false;
	}

	frame.id = static_cast<ID>(copy[4]);

	memcpy(frame.payload, &copy[5], frame.length);

	return true;
}

uint8_t Transfer::calculate_CRC(const uint8_t *buffer, const size_t size) {
	uint8_t crc = 0xff;

    for(size_t i=0; i<size; i++) {
        crc ^=buffer[i];

        for(size_t j=0; j<8; j++) {

            if((crc & 0x80)!=0) {
                crc = (uint8_t)((crc<<1)^0x31);
            } else {
                crc <<=1;
			}
        }
    }

    return crc;
}

uint8_t Transfer::encode_COBS(uint8_t *buffer, const size_t size) {
	uint8_t cobs = 1;

	for(int i=size-1; i>=0; i--) {
		if(buffer[i]!=start_byte) {
			cobs++;
			continue;
		}

		buffer[i] = cobs;
		cobs = 1;
	}

	return cobs;
}

void Transfer::decode_COBS(uint8_t *buffer, const size_t size, const uint8_t cobs) {

	size_t index = cobs - 1;

	while(index<size) {
		const int next = buffer[index];

		buffer[index] = start_byte;

		index +=next;
	}
}

Transfer::FrameTX Transfer::encode(const void *payload, const size_t length, const ID id) {
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
