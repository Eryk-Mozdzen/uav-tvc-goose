#pragma once

#include "extended_kalman_filter.h"
#include "vector.h"
#include "quaternion.h"
#include "queue_element.h"
#include "TaskCPP.h"

class StateEstimator : TaskClassS<1024> {
		static constexpr float dt = 0.005;

		ExtendedKalmanFilter<7, 3, 6> ekf;
		Quaternion body_to_world;

		Vector acceleration;
		Vector magnetic_field;
		bool acc_ready;
		bool mag_ready;

		Quaternion getAttitudeNED() const;
		Vector removeMagneticDeclination(Vector mag) const;

		void zeroAzimuth();

	public:
		StateEstimator();

		void handleReading(const queue_element_t reading);

		Quaternion getAttitude() const;

		void task();
};
