#pragma once

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"

class IMU_9DoF {
	public:
		struct Vector3i {
			int16_t x, y, z;
		};
		struct Vector3f {
			float x, y, z;
		};
	
	private:
		struct Accel {
			static constexpr float scale = 1.6*16/32768;
			static constexpr Vector3f offset = {500, 380, -750};
		};
		struct Gyro {
			//static constexpr float scale = 1;
			//static constexpr Vector3f offset = {0, 0, 0};
			static constexpr float scale = 0.1*1/8.75;
			static constexpr Vector3f offset = {210, 20, -30};
		};
		struct Mag {
			static constexpr Vector3f scale = {1, 1, 1};
			static constexpr Vector3f offset = {0, 0, 0};
			//static constexpr Vector3f scale = {0.0039166/1.84, 0.003657/1.84, 0.09832/1.84};
			//static constexpr Vector3f offset = {76, 200, 10};
		};

		I2C_HandleTypeDef &hi2c;
		Vector3i acceleration;
		Vector3i gyration;
		Vector3i magnetic_field;
		SemaphoreHandle_t lock;

		static void task(void *);
		void read();
	
	public:
		IMU_9DoF(I2C_HandleTypeDef &hi2c);
		~IMU_9DoF();

		bool initializeSensors();
		void startSampling();

		Vector3i getRawAcceleration();
		Vector3i getRawGyration();
		Vector3i getRawMagneticField();

		Vector3f getAcceleration();
		Vector3f getGyration();
		Vector3f getMagneticField();
};
