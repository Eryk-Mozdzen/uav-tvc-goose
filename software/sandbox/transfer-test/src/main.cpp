#include <transfer/transfer.h>
#include <cstdio>
#include <cstring>
#include <cassert>

void print(uint8_t place, uint8_t *buffer, uint8_t size) {

	for(int i=0; i<place; i++) {
		printf("   ");
	}

	for(int i=0; i<size; i++) {
		printf("%02X ", buffer[i]);
	}
	printf("\n");
}

template<typename T>
void test(T data) {
	uint8_t raw[255];
	memcpy(raw, &data, sizeof(T));
	print(5, raw, sizeof(T));

	uint8_t buffer[255];
	uint8_t size = Transfer::encode(buffer, data, 13);
	print(0, buffer, size);

	assert(buffer[0]==0);
	for(int i=1; i<size; i++) {
		assert(buffer[i]!=0);
	}

	Transfer transfer;

	for(int i=0; i<size; i++) {
		transfer.consume(buffer[i]);

		uint8_t id;
		uint8_t decoded[255];
		size_t len;

		if(transfer.receive(id, decoded, len)) {
			print(5, decoded, len);
		}
	}
}

int main() {

	test(1000.f);
	test(0.f);
	test(-3.456f);

	test(1000.);
	test(0.);
	test(-3.456);

	test((uint8_t)123);
	test((uint8_t)0);
	test((uint8_t)255);
	test((uint8_t)-34);
	test((uint8_t)1000);

	test((uint16_t)123);
	test((int16_t)123);

	struct Vector3f {
		float x, y, z;
	};
	struct Vector3i {
		int16_t x, y, z;
	};
	struct SensorReading {
		Vector3f calibrated;
		Vector3i raw;
	};

	test(SensorReading{0, 12.2435, 21231.5, -123, 1023, 800});
}
