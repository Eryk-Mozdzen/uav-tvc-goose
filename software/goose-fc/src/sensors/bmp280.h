#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>

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

typedef struct {
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
} bmp280_calib_data_t;

typedef uint32_t BMP280_U32_t;
typedef int32_t BMP280_S32_t;
typedef int64_t BMP280_S64_t;

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P);

#endif
