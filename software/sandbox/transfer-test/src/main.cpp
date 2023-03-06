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

	Transfer::FrameTX tx = Transfer::encode(data, Transfer::TELEMETRY_ESTIMATION_ALTITUDE);

	print(0, tx.buffer, tx.length);

	assert(tx.buffer[0]==0);
	for(size_t i=1; i<tx.length; i++) {
		assert(tx.buffer[i]!=0);
	}

	Transfer transfer;

	for(size_t i=0; i<tx.length; i++) {
		transfer.consume(tx.buffer[i]);

		Transfer::FrameRX rx;

		if(transfer.receive(rx)) {

			T recv;
			assert(rx.getPayload(recv));

			uint8_t decoded[255];
			memcpy(decoded, &recv, sizeof(T));
			print(5, decoded, sizeof(T));

			for(size_t i=0; i<sizeof(T); i++) {
				assert(decoded[i]==raw[i]);
			}
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

	struct __attribute__((packed)) Vector3f {
		float x, y, z;
	};
	struct __attribute__((packed)) Vector3i {
		int16_t x, y, z;
	};
	struct __attribute__((packed)) SensorReading {
		Vector3f calibrated;
		Vector3i raw;
	};

	test(SensorReading{0, 12.2435, 21231.5, -123, 1023, 800});
	test(SensorReading{0, 0, 0, 0, 0, 0});
	test(SensorReading{45, -12.2435, 0.545645645, -123, -1023, -800});
}
