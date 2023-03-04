#pragma once

#include "multi_queue.h"
#include "vector.h"
#include "message.h"

class Transport {
	Transport();

public:

	enum TX {
		LOG,
		TELEMETRY
	};

	enum Sensors {
		ACCELEROMETER,
		GYROSCOPE,
		MAGNETOMETER,
		BAROMETER
	};

	MultiQueue<Sensors, 16, Vector, float> sensor_queue;
	MultiQueue<TX, 32, Message> tx_queue;

	Transport(Transport &) = delete;
	void operator=(const Transport &) = delete;

	static Transport & getInstance();
};
