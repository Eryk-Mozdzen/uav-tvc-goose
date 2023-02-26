#pragma once

#include "TaskCPP.h"
#include <cstdint>

class BMP280 : public TaskClassS<512> {

	struct Calibration {
		uint16_t dig_T1;
		int16_t dig_T2;
		int16_t dig_T3;

		uint16_t dig_P1;
		int16_t dig_P2;
		int16_t dig_P3;
		int16_t dig_P4;
		int16_t dig_P5;
		int16_t dig_P6;
		int16_t dig_P7;
		int16_t dig_P8;
		int16_t dig_P9;
	};

	Calibration calib;

	float pressure;	
	float temperature;

	using BMP280_U32_t = uint32_t;
	using BMP280_S32_t = int32_t;
	using BMP280_S64_t = int64_t;

	BMP280_S32_t t_fine;
	BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
	BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P);

	public:

		BMP280();

		void init();
		bool readCalibrationData();

		bool readData();
		float getPressure() const;
		float getTemperature() const;

		void task();
};
