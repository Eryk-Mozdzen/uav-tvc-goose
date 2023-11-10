#pragma once

#include <cstdint>

namespace comm {

    enum Command : uint8_t {
		START,
		LAND,
		ABORT
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

			static State zero() {
				State state;
				state.rpy[0] = 0;
				state.rpy[1] = 0;
				state.rpy[2] = 0;
				state.w[0] = 0;
				state.w[1] = 0;
				state.w[2] = 0;
				state.z = 0;
				state.vz = 0;
				return state;
			}
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
