#include "stm32f4xx_hal.h"
#include "TaskCPP.h"

#include "logger.h"
#include "transport.h"
#include "sensor_bus.h"
#include "matrix.h"
#include "interval_logger.h"

#define HMC5883L_ADDR				0x1E

#define HMC5883L_REG_CONFIG_A		0x00
#define HMC5883L_REG_CONFIG_B		0x01
#define HMC5883L_REG_MODE			0x02
#define HMC5883L_REG_DATA_X_MSB		0x03
#define HMC5883L_REG_DATA_X_LSB		0x04
#define HMC5883L_REG_DATA_Z_MSB		0x05
#define HMC5883L_REG_DATA_Z_LSB		0x06
#define HMC5883L_REG_DATA_Y_MSB		0x07
#define HMC5883L_REG_DATA_Y_LSB		0x08
#define HMC5883L_REG_STATUS			0x09
#define HMC5883L_REG_IDENT_A		0x10
#define HMC5883L_REG_IDENT_B		0x11
#define HMC5883L_REG_IDENT_C		0x12

#define HMC5883L_CONFIG_A_SAMPLES_1		(0x00<<5)
#define HMC5883L_CONFIG_A_SAMPLES_2		(0x01<<5)
#define HMC5883L_CONFIG_A_SAMPLES_4		(0x02<<5)
#define HMC5883L_CONFIG_A_SAMPLES_8 	(0x03<<5)
#define HMC5883L_CONFIG_A_RATE_0_75HZ	(0x00<<2)
#define HMC5883L_CONFIG_A_RATE_1_5HZ	(0x01<<2)
#define HMC5883L_CONFIG_A_RATE_3HZ		(0x02<<2)
#define HMC5883L_CONFIG_A_RATE_7_5HZ	(0x03<<2)
#define HMC5883L_CONFIG_A_RATE_15HZ		(0x04<<2)
#define HMC5883L_CONFIG_A_RATE_30HZ		(0x05<<2)
#define HMC5883L_CONFIG_A_RATE_75		(0x06<<2)
#define HMC5883L_CONFIG_A_MEAS_NORMAL	(0x00<<0)
#define HMC5883L_CONFIG_A_MEAS_POSITIVE	(0x01<<0)
#define HMC5883L_CONFIG_A_MEAS_NEGATIVE	(0x02<<0)

#define HMC5883L_CONFIG_B_RANGE_0_88GA	(0x00<<5)
#define HMC5883L_CONFIG_B_RANGE_1_3GA	(0x01<<5)
#define HMC5883L_CONFIG_B_RANGE_1_9GA	(0x02<<5)
#define HMC5883L_CONFIG_B_RANGE_2_5GA	(0x03<<5)
#define HMC5883L_CONFIG_B_RANGE_4GA		(0x04<<5)
#define HMC5883L_CONFIG_B_RANGE_4_7GA	(0x05<<5)
#define HMC5883L_CONFIG_B_RANGE_5_6GA	(0x06<<5)
#define HMC5883L_CONFIG_B_RANGE_8_1GA	(0x07<<5)

#define HMC5883L_MODE_CONTINOUS			(0x00<<0)
#define HMC5883L_MODE_SINGLE			(0x01<<0)
#define HMC5883L_MODE_IDLE				(0x02<<0)

class HMC5883L : public TaskClassS<512> {
	Vector magnetic_field;

	IntervalLogger<Vector> telemetry;

public:

	HMC5883L();

	void init();

	bool readData();
	Vector getMagneticField() const;

	void task();
};

TaskHandle_t mag_task;

HMC5883L magnetometer;

HMC5883L::HMC5883L() : TaskClassS{"HMC5883L reader", TaskPrio_Low},
		telemetry{"HMC5883L telemetry", Transfer::ID::TELEMETRY_SENSOR_MAGNETIC_FIELD} {

}

void HMC5883L::init() {

	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	SensorBus::getInstance().write(HMC5883L_ADDR, HMC5883L_REG_CONFIG_A,
		HMC5883L_CONFIG_A_MEAS_NORMAL |
		HMC5883L_CONFIG_A_RATE_75 |
		HMC5883L_CONFIG_A_SAMPLES_8
	);

	SensorBus::getInstance().write(HMC5883L_ADDR, HMC5883L_REG_CONFIG_B,
		HMC5883L_CONFIG_B_RANGE_1_3GA
	);

	SensorBus::getInstance().write(HMC5883L_ADDR, HMC5883L_REG_MODE,
		HMC5883L_MODE_CONTINOUS
	);

	Logger::getInstance().log(Logger::INFO, "mag: initialization complete");
}

bool HMC5883L::readData() {
	uint8_t buffer[6] = {0};

	if(SensorBus::getInstance().read(HMC5883L_ADDR, HMC5883L_REG_DATA_X_MSB, buffer, sizeof(buffer))) {
		return false;
	}

	const int16_t raw_x = (((int16_t)buffer[0])<<8) | buffer[1];
	const int16_t raw_y = (((int16_t)buffer[4])<<8) | buffer[5];
	const int16_t raw_z = (((int16_t)buffer[2])<<8) | buffer[3];

	if((raw_x>2047 || raw_x<-2048) || (raw_y>2047 || raw_y<-2048) || (raw_z>2047 || raw_z<-2048)) {
		Logger::getInstance().log(Logger::WARNING, "mag: value out of valid range [%10d %10d %10d]", raw_x, raw_y, raw_z);
	}

	constexpr float gain = 1090.f;

	const Vector mag_raw = Vector(raw_x, raw_y, raw_z)/gain;

	const Vector offset = {0.04157f, -0.10032f, -0.06273f};
	const Matrix<3, 3> scale = {
		1.98471f, 0,		0,
		0,		  2.07220f, 0,
		0,		  0,		2.31329f
	};

	magnetic_field = scale*(mag_raw - offset);

	return true;
}

Vector HMC5883L::getMagneticField() const {
	return magnetic_field;
}

void HMC5883L::task() {
	mag_task = getTaskHandle();

	init();

	while(1) {
		ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY);

		if(!readData()) {
			continue;
		}

		const Vector mag = getMagneticField();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::MAGNETOMETER, mag, 0);

		telemetry.feed(mag);
	}
}
