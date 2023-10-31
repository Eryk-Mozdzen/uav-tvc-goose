#pragma once

#include <cstdint>

namespace comm {

    enum Command : uint8_t {
		START,
		LAND,
		ABORT,
		RESET
	};

	struct Estimator {
		float attitude[4];
		float position[3];
		float velocity[3];
		float acceleration[3];
		float soc;
		float ground_pressure;
	};

	struct Controller {
		struct State {
			float rpy[3];
			float w[3];
			float z;
			float vz;
		};

		enum SMState : uint32_t {
			ABORT,
			READY,
			ACTIVE,
			LANDING,
			MANUAL
		};

		State setpoint;
		State process_value;
		float angles[4];
		float throttle;
		SMState state;
	};

	struct Manual {

		enum Type : uint32_t {
			NORMAL,
			CALIBRATION
		};

		Type type;
		float throttle;
		float angles[4];
	};

	struct Power {
		float shunt;
		float bus;
		float current;
		float power;
	};

}
