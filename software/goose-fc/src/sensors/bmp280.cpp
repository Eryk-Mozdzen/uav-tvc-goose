#include "bmp280.h"

#include <cstdlib>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "communication.h"
#include "queue_element.h"
#include "sensors.h"
#include "sensor_bus.h"

#define BMP280_ADDR				0x77

#define BMP280_REG_TEMP_XLSB	0xFC
#define BMP280_REG_TEMP_LSB		0xFB
#define BMP280_REG_TEMP_MSB		0xFA
#define BMP280_REG_PRESS_XLSB	0xF9
#define BMP280_REG_PRESS_LSB	0xF8
#define BMP280_REG_PRESS_MSB	0xF7
#define BMP280_REG_CONFIG		0xF5
#define BMP280_REG_CTRL_MEAS	0xF4
#define BMP280_REG_STATUS		0xF3
#define BMP280_REG_RESET		0xE0
#define BMP280_REG_ID			0xD0

#define BMP280_REG_CALIB25		0xA1
#define BMP280_REG_CALIB24		0xA0
#define BMP280_REG_CALIB23		0x9F
#define BMP280_REG_CALIB22		0x9E
#define BMP280_REG_CALIB21		0x9D
#define BMP280_REG_CALIB20		0x9C
#define BMP280_REG_CALIB19		0x9B
#define BMP280_REG_CALIB18		0x9A
#define BMP280_REG_CALIB17		0x99
#define BMP280_REG_CALIB16		0x98
#define BMP280_REG_CALIB15		0x97
#define BMP280_REG_CALIB14		0x96
#define BMP280_REG_CALIB13		0x95
#define BMP280_REG_CALIB12		0x94
#define BMP280_REG_CALIB11		0x93
#define BMP280_REG_CALIB10		0x92
#define BMP280_REG_CALIB09		0x91
#define BMP280_REG_CALIB08		0x90
#define BMP280_REG_CALIB07		0x8F
#define BMP280_REG_CALIB06		0x8E
#define BMP280_REG_CALIB05		0x8D
#define BMP280_REG_CALIB04		0x8C
#define BMP280_REG_CALIB03		0x8B
#define BMP280_REG_CALIB02		0x8A
#define BMP280_REG_CALIB01		0x89
#define BMP280_REG_CALIB00		0x88

#define BMP280_ID_VALUE			0x58

#define BMP280_RESET_VALUE		0xB6

#define BMP280_CTRL_TEMP_OVERSAMPLING_SKIPPED	(0x00<<5)
#define BMP280_CTRL_TEMP_OVERSAMPLING_1			(0x01<<5)
#define BMP280_CTRL_TEMP_OVERSAMPLING_2			(0x02<<5)
#define BMP280_CTRL_TEMP_OVERSAMPLING_4			(0x03<<5)
#define BMP280_CTRL_TEMP_OVERSAMPLING_8			(0x04<<5)
#define BMP280_CTRL_TEMP_OVERSAMPLING_16		(0x05<<5)
#define BMP280_CTRL_PRESS_OVERSAMPLING_SKIPPED	(0x00<<2)
#define BMP280_CTRL_PRESS_OVERSAMPLING_1		(0x01<<2)
#define BMP280_CTRL_PRESS_OVERSAMPLING_2		(0x02<<2)
#define BMP280_CTRL_PRESS_OVERSAMPLING_4		(0x03<<2)
#define BMP280_CTRL_PRESS_OVERSAMPLING_8		(0x04<<2)
#define BMP280_CTRL_PRESS_OVERSAMPLING_16		(0x05<<2)
#define BMP280_CTRL_MODE_SLEEP					(0x00<<0)
#define BMP280_CTRL_MODE_FORCED					(0x01<<0)
#define BMP280_CTRL_MODE_NORMAL					(0x03<<0)

#define BMP280_CONFIG_STANDBY_0_5MS				(0x00<<5)
#define BMP280_CONFIG_STANDBY_62_5MS			(0x01<<5)
#define BMP280_CONFIG_STANDBY_125MS				(0x02<<5)
#define BMP280_CONFIG_STANDBY_250MS				(0x03<<5)
#define BMP280_CONFIG_STANDBY_500MS				(0x04<<5)
#define BMP280_CONFIG_STANDBY_1000MS			(0x05<<5)
#define BMP280_CONFIG_STANDBY_2000MS			(0x06<<5)
#define BMP280_CONFIG_STANDBY_4000MS			(0x07<<5)
#define BMP280_CONFIG_FILTER_OFF				(0x00<<2)
#define BMP280_CONFIG_FILTER_X2					(0x01<<2)
#define BMP280_CONFIG_FILTER_X4					(0x02<<2)
#define BMP280_CONFIG_FILTER_X8					(0x03<<2)
#define BMP280_CONFIG_FILTER_X16				(0x04<<2)
#define BMP280_CONFIG_SPI_3WIRE_DISABLE			(0x00<<0)
#define BMP280_CONFIG_SPI_3WIRE_ENABLE			(0x01<<0)

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280::BMP280_S32_t BMP280::bmp280_compensate_T_int32(BMP280_S32_t adc_T) {
	BMP280_S32_t var1, var2, T;

	var1 = ((((adc_T>>3) - ((BMP280_S32_t)calib.dig_T1<<1))) * ((BMP280_S32_t)calib.dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BMP280_S32_t)calib.dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)calib.dig_T1))) >> 12) * ((BMP280_S32_t)calib.dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;

	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP280::BMP280_U32_t BMP280::bmp280_compensate_P_int64(BMP280_S32_t adc_P) {
	BMP280_S64_t var1, var2, p;

	var1 = ((BMP280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BMP280_S64_t)calib.dig_P6;
	var2 = var2 + ((var1*(BMP280_S64_t)calib.dig_P5)<<17);
	var2 = var2 + (((BMP280_S64_t)calib.dig_P4)<<35);
	var1 = ((var1 * var1 * (BMP280_S64_t)calib.dig_P3)>>8) + ((var1 * (BMP280_S64_t)calib.dig_P2)<<12);
	var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)calib.dig_P1)>>33;

	if(var1==0) {
		return 0; // avoid exception caused by division by zero
	}

	p = 1048576 - adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BMP280_S64_t)calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BMP280_S64_t)calib.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)calib.dig_P7)<<4);

	return (BMP280_U32_t)p;
}

BMP280::BMP280() : pressure{0}, temperature{0} {

	memset(&calib, 0, sizeof(Calibration));
}

void BMP280::init() {

	SensorBus::getInstance().write(BMP280_ADDR, BMP280_REG_RESET, BMP280_RESET_VALUE);

	SensorBus::getInstance().write(BMP280_ADDR, BMP280_REG_CTRL_MEAS, 
		BMP280_CTRL_TEMP_OVERSAMPLING_2 |
		BMP280_CTRL_PRESS_OVERSAMPLING_16 |
		BMP280_CTRL_MODE_NORMAL
	);

	SensorBus::getInstance().write(BMP280_ADDR, BMP280_REG_CONFIG,
		BMP280_CONFIG_STANDBY_0_5MS |
		BMP280_CONFIG_FILTER_X16 |
		BMP280_CONFIG_SPI_3WIRE_DISABLE
	);
}

bool BMP280::readCalibrationData() {

	uint8_t buffer[24] = {0};

	if(SensorBus::getInstance().read(BMP280_ADDR, BMP280_REG_CALIB00, buffer, sizeof(buffer))) {
		COM.log(Communication::ERROR, "bar: error in calibration values\n\r");
		return false;
	}
	
	calib.dig_T1 = (((uint16_t)buffer[1])<<8) | buffer[0];
	calib.dig_T2 = (((int16_t)buffer[3])<<8) | buffer[2];
	calib.dig_T3 = (((int16_t)buffer[5])<<8) | buffer[4];

	calib.dig_P1 = (((uint16_t)buffer[7])<<8) | buffer[6];
	calib.dig_P2 = (((int16_t)buffer[9])<<8) | buffer[8];
	calib.dig_P3 = (((int16_t)buffer[11])<<8) | buffer[10];
	calib.dig_P4 = (((int16_t)buffer[13])<<8) | buffer[12];
	calib.dig_P5 = (((int16_t)buffer[15])<<8) | buffer[14];
	calib.dig_P6 = (((int16_t)buffer[17])<<8) | buffer[16];
	calib.dig_P7 = (((int16_t)buffer[19])<<8) | buffer[18];
	calib.dig_P8 = (((int16_t)buffer[21])<<8) | buffer[20];
	calib.dig_P9 = (((int16_t)buffer[23])<<8) | buffer[22];

	COM.log(Communication::INFO, "bar: succesfully read calibration values\n\r");

	return true;
}

bool BMP280::readData() {
	uint8_t buffer[6] = {0};

	if(SensorBus::getInstance().read(BMP280_ADDR, BMP280_REG_PRESS_MSB, buffer, sizeof(buffer))) {
		return false;
	}

	const int32_t raw_temperature  = (((int32_t)buffer[3])<<12) | (((int32_t)buffer[4])<<4) | (((int32_t)buffer[5])>>4);
	const int32_t raw_pressure     = (((int32_t)buffer[0])<<12) | (((int32_t)buffer[1])<<4) | (((int32_t)buffer[2])>>4);

	temperature  = bmp280_compensate_T_int32(raw_temperature)/100.f;	// *C
	pressure     = bmp280_compensate_P_int64(raw_pressure)/256.f;		// Pa

	return true;
}

float BMP280::getPressure() const {
	return pressure;
}

float BMP280::getTemperature() const {
	return temperature;
}

void barTaskFcn(void *param) {
	QueueHandle_t sensor_queue = static_cast<QueueHandle_t>(param);

	BMP280 barometer;

	barometer.init();
	barometer.readCalibrationData();

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, 100);
		
		if(!barometer.readData()) {
			continue;
		}

		const float press = barometer.getPressure();

		queue_element_t reading;
		reading.type = Sensors::BAROMETER;
		memcpy(reading.data, &press, sizeof(float));

		xQueueSend(sensor_queue, &reading, 0);
	}
}
