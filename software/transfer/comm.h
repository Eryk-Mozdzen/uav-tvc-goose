#pragma once

namespace comm {

    enum Command : uint8_t {
		START,
		LAND
	};

	struct Estimator {
		enum AltitudeSource {
			DISTANCE,
			PRESSURE
		};

		float quat[4];
		float linear[3];
		float z, vz;
		float battery_soc;
		AltitudeSource altitude_src;
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
			TAKE_OFF,
			ACTIVE,
			LANDING
		};

		State setpoint;
		State process_value;
		float angles[4];
		float throttle;
		SMState state;
	};

}
