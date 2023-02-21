#include "state_estimator.h"

#include <cmath>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "logger.h"
#include "queue_element.h"
#include "matrix.h"
#include "kalman_filter.h"
#include "extended_kalman_filter.h"

QueueHandle_t state_queue;

static float3_t acceleration;
static float3_t gyration;
static float3_t magnetic_field;
static float pressure;

void handle_readings() {
	queue_element_t reading;

	while(xQueueReceive(sensor_queue, &reading, 0)) {

		switch(reading.type) {
			case SENSOR_ACCELEROMETER: {
				memcpy(&acceleration, reading.data, sizeof(float3_t));

			} break;
			case SENSOR_GYROSCOPE: {
				memcpy(&gyration, reading.data, sizeof(float3_t));

			} break;
			case SENSOR_MAGNETOMETER: {
				memcpy(&magnetic_field, reading.data, sizeof(float3_t));

			} break;
			case SENSOR_BAROMETER: {
				memcpy(&pressure, reading.data, sizeof(float));

			} break;
			default: {
				LOG(LOG_WARNING, "est: unknown sensor reading\n\r");
			} break;
		}

		vPortFree(reading.data);
	}
}

float3_t normalize(float3_t vec) {
	const float len = sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	
	if(len<0.01f)
		return vec;

	vec.x /=len;
	vec.y /=len;
	vec.z /=len;
	
	return vec;
}

float3_t get_attitude(float3_t acc, float3_t mag) {
	acc = normalize(acc);
	mag = normalize(mag);

	const float roll  = atan2f(acc.y, acc.z);
	const float pitch = atan2f(-acc.x, sqrtf(acc.y*acc.y + acc.z*acc.z));

	const float cos_roll = cosf(roll);
	const float sin_roll = sinf(roll);
	const float cos_pitch = cosf(pitch);
	const float sin_pitch = sinf(pitch);

	const float yaw = atan2f(
		-mag.y*cos_roll + mag.z*sin_roll, 
		mag.x*cos_pitch + mag.y*sin_pitch*sin_roll + mag.z*sin_pitch*cos_roll
	);

	float3_t attitude;
	attitude.x = roll;
	attitude.y = pitch;
	attitude.z = yaw;
	
	return attitude;
}

KalmanFilter<6, 3, 3> get_kf() {
	constexpr float dt = 0.01;

	constexpr Matrix<6, 6> F = {
		1, 0, 0, dt, 0,  0,
		0, 1, 0, 0,  dt, 0,
		0, 0, 1, 0,  0,  dt, 
		0, 0, 0, 1,  0,  0, 
		0, 0, 0, 0,  1,  0,
		0, 0, 0, 0,  0,  1
	};

	constexpr Matrix<6, 3> B = {
		dt, 0,  0,
		0,  dt, 0,
		0,  0,  dt,
		0,  0,  0,
		0,  0,  0,
		0,  0,  0
	};

	constexpr Matrix<3, 6> H = {
		1, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 0, 0,
		0, 0, 1, 0, 0, 0
	};

	constexpr Matrix<6, 6> Q = Matrix<6, 6>::identity()*0.005f;
	constexpr Matrix<3, 3> R = Matrix<3, 3>::identity()*1.f;

	return KalmanFilter(F, B, H, Q, R, {0, 0, 0, 0, 0, 0});
}

ExtendedKalmanFilter<6, 3, 3> get_ekf() {

	constexpr float dt = 0.01;

	const std::function<Matrix<6, 1>(const Matrix<6, 1>, const Matrix<3, 1>)> f = [](const Matrix<6, 1> x, const Matrix<3, 1> u) {

		const float roll = x(0, 0);
		const float pitch = x(1, 0);
		const float yaw = x(2, 0);

		const float cos_roll = cosf(roll);
		const float sin_roll = sinf(roll);
		const float tan_roll = tanf(roll);
		const float cos_pitch = cosf(pitch);
		const float sin_pitch = sinf(pitch);
		const float tan_pitch = tanf(pitch);

		const float wx = u(0, 0);
		const float wy = u(1, 0);
		const float wz = u(2, 0);

		const Matrix<6, 1> x_new = {
			roll + dt*(wx + wy*sin_roll*tan_pitch + wz*cos_roll*tan_pitch),
			pitch + dt*(wy*cos_roll - wz*sin_roll),
			yaw + dt*wz,
			wx,
			wy,
			wz
		};
		
		return x_new;
	};

	const std::function<Matrix<3, 1>(const Matrix<6, 1>)> h = [](const Matrix<6, 1> x) {
		const Matrix<3, 1> z;
		
		return z;
	};

	const std::function<Matrix<6, 6>(const Matrix<6, 1>, const Matrix<3, 1>)> d_dx_f = [](const Matrix<6, 1> x, const Matrix<3, 1> u) {
		const float wx = u(0, 0);
		const float wy = u(1, 0);
		const float wz = u(2, 0);
		
		const Matrix<6, 6> F;
		
		return F;
	};

	const std::function<Matrix<3, 6>(const Matrix<6, 1>)> d_dx_h = [](const Matrix<6, 1> x) {
		const Matrix<3, 6> H;
		
		return H;
	};

	constexpr Matrix<6, 6> Q = Matrix<6, 6>::identity()*0.005f;
	constexpr Matrix<3, 3> R = Matrix<3, 3>::identity()*1.f;

	return ExtendedKalmanFilter(f, h, d_dx_f, d_dx_h, Q, R, {0, 0, 0, 0, 0, 0});
}

void estimator(void *param) {
	(void)param;

	TickType_t time = xTaskGetTickCount();

	constexpr float pi = 3.1415f;
	constexpr float rad_to_deg = 180.f/pi;

	KalmanFilter kf = get_kf();

	while(1) {
		xTaskDelayUntil(&time, 10);

		handle_readings();

		kf.predict({gyration.x, gyration.y, gyration.z});

		const float3_t observation = get_attitude(acceleration, magnetic_field);
		kf.update({observation.x, observation.y, observation.z});

		const Matrix<6, 1> kf_attitude = kf.getState();

		LOG(LOG_DEBUG, "kf: %7.2f %7.2f %7.2f\n\r", (double)(kf_attitude(0, 0)*rad_to_deg), (double)(kf_attitude(1, 0)*rad_to_deg), (double)(kf_attitude(2, 0)*rad_to_deg));
	}
}

void StateEstimator_Init() {
	state_queue = xQueueCreate(16, sizeof(state_t));

	xTaskCreate(estimator, "state estimator", 1024, NULL, 4, NULL);
}
