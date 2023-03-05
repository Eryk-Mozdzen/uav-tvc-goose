#include <transfer/transfer.h>

Transfer::Transfer() {
	reset();
}

void Transfer::reset() {
	counter = 0;

	memset(frame, 0xFF, sizeof(frame));
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

	frame[counter] = byte;
	counter++;
}

bool Transfer::receive(uint8_t &id, uint8_t *buffer, size_t &length) const {
	if(counter<5) {
		return false;
	}

	if(frame[0]!=start_byte) {
		return false;
	}

	memcpy(buffer, frame, counter);

	const uint8_t cobs = buffer[1];

	decode_COBS(&buffer[2], counter - 2, cobs);

	length = buffer[3];
	const uint8_t length_real = counter - 5;

	if(length!=length_real) {
		return false;
	}

	const uint8_t crc = buffer[2];
	const uint8_t crc_real = calculate_CRC(&buffer[4], length + 1);

	if(crc!=crc_real) {
		return false;
	}

	id = buffer[4];
	memmove(buffer, &buffer[5], length);

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

	int index = cobs - 1;

	while(index<size) {
		const int next = buffer[index];
		
		buffer[index] = start_byte;
		
		index +=next;
	}
}
