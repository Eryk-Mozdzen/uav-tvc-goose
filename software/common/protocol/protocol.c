#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "protocol.h"

static uint8_t crc8(const uint8_t *buffer, const size_t size) {
    assert(buffer);

    uint8_t crc = 0xFF;

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

static uint8_t cobs_encode(uint8_t *buffer, const size_t size) {
    assert(buffer);

	uint8_t cobs = 1;

	for(size_t i=0; i<size; i++) {
		if(buffer[size-i-1]) {
			cobs++;
		} else {
            buffer[size-i-1] = cobs;
		    cobs = 1;
        }
	}

	return cobs;
}

static void cobs_decode(uint8_t *buffer, const size_t size, const uint8_t cobs) {
    assert(buffer);

	size_t index = cobs - 1;
    uint8_t next;

	while(index<size) {
		next = buffer[index];
		buffer[index] = 0;
		index +=next;
	}
}

size_t protocol_encode(void *dest, const protocol_message_t *message) {
    assert(dest);
    assert(message);
    assert(message->size<=250);
    assert(message->payload || (!message->payload && message->size));

    uint8_t *buffer = (uint8_t *)dest;

    buffer[message->size + 4] = 0;

    if(message->payload) {
        memcpy(&buffer[4], message->payload, message->size);
    }

    buffer[3] = message->id;
    buffer[2] = message->size;
    buffer[1] = crc8(&buffer[2], message->size + 2);
    buffer[0] = cobs_encode(&buffer[1], message->size + 3);

    return message->size + 5;
}

bool protocol_decode(protocol_decoder_t *decoder, const uint8_t byte, protocol_message_t *message) {
    assert(decoder);
    assert(decoder->buffer);
    assert(decoder->size);
    assert(message);

    decoder->buffer[decoder->counter] = byte;
    decoder->counter++;

    if(byte || (decoder->counter>=decoder->size)) {
        decoder->counter = 0;
        return false;
    }

    cobs_decode(&decoder->buffer[1], decoder->counter - 1, decoder->buffer[0]);

    if(decoder->buffer[2]!=(decoder->counter - 5)) {
        decoder->counter = 0;
    	return false;
    }

	if(decoder->buffer[1]!=crc8(&decoder->buffer[2], decoder->counter - 2)) {
        decoder->counter = 0;
		return false;
	}

    message->payload = &decoder->buffer[4];
    message->id = decoder->buffer[3];
    message->size = decoder->buffer[2];

    decoder->counter = 0;
	return true;
}
