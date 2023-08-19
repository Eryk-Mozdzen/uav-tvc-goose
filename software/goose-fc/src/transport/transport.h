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
		BAROMETER,
		LASER,
		VOLTAGE,
		CURRENT,
		HALL
	};

	MultiQueue<Sensors, 16, Vector, float> sensor_queue;
	Queue<uint8_t, 512> wire_rx_queue;

	Queue<Transfer::FrameTX, 16> wire_tx_queue;
	Queue<Transfer::FrameTX, 16> wireless_tx_queue;
	Queue<Transfer::FrameRX, 16> frame_rx_queue;
	Queue<comm::Controller::State, 16> state_queue;

	Transport(Transport &) = delete;
	void operator=(const Transport &) = delete;

	static Transport & getInstance();
};
