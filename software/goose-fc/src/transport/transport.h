#pragma once

#include "multi_queue.h"
#include "vector.h"
#include "message.h"
#include "transfer.h"

class Transport {
	Transport();

public:

	enum Sensors {
		ACCELEROMETER,
		GYROSCOPE,
		MAGNETOMETER,
		BAROMETER
	};

	MultiQueue<Sensors, 16, Vector, float> sensor_queue;
	Queue<Transfer::FrameTX, 32> wire_tx_queue;
	Queue<uint8_t, 512> wire_rx_queue;
	Queue<Transfer::FrameTX, 32> wireless_tx_queue;
	Queue<Transfer::FrameRX, 32> frame_rx_queue;

	Transport(Transport &) = delete;
	void operator=(const Transport &) = delete;

	static Transport & getInstance();
};
