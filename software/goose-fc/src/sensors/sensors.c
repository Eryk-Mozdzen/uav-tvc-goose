#include "sensors.h"

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "logger.h"
#include "queue_element.h"
#include <string.h>

#define IMU_ADDRESS		0x68
#define MAG_ADDRESS		0x1E
#define BAR_ADDRESS		0x77

static SemaphoreHandle_t bus_lock;

I2C_HandleTypeDef hi2c1;
QueueHandle_t sensor_queue;

void write_reg(const uint8_t device, const uint8_t reg, uint8_t src) {
	xSemaphoreTake(bus_lock, portMAX_DELAY);
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, device<<1, reg, 1, &src, 1, 100);
	if(status!=HAL_OK) {
		LOG(LOG_ERROR, "sensors: unable to write into 0x%02X device\n\r", device);
	}
	xSemaphoreGive(bus_lock);
}

void read_regs(const uint8_t device, const uint8_t reg, uint8_t *dest, const uint8_t len) {
	xSemaphoreTake(bus_lock, portMAX_DELAY);
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, device<<1, reg, 1, dest, len, 100);
	if(status!=HAL_OK) {
		LOG(LOG_ERROR, "sensors: unable to read from 0x%02X device\n\r", device);
	}
	xSemaphoreGive(bus_lock);
}

void read_imu(void *param) {
	(void)param;

	write_reg(IMU_ADDRESS, 0x6B, 0x00);
	write_reg(IMU_ADDRESS, 0x1C, 0x10);
	write_reg(IMU_ADDRESS, 0x1B, 0x10);

	TickType_t time = xTaskGetTickCount();

	uint8_t buffer[14];

	queue_element_t reading;

	while(1) {
		vTaskDelayUntil(&time, 100);

		read_regs(IMU_ADDRESS, 0x3B, buffer, sizeof(buffer));

		const int16_t acc_raw_x = (((int16_t)buffer[0])<<8) | buffer[1];
		const int16_t acc_raw_y = (((int16_t)buffer[2])<<8) | buffer[3];
		const int16_t acc_raw_z = (((int16_t)buffer[4])<<8) | buffer[5];

		const float3_t acc = {
			.x = acc_raw_x,
			.y = acc_raw_y,
			.z = acc_raw_z
		};

		reading.type = SENSOR_ACCELEROMETER;
		reading.data = pvPortMalloc(sizeof(float3_t));
		memcpy(reading.data, &acc, sizeof(float3_t));
		xQueueSend(sensor_queue, &reading, 0);

		const int16_t gyr_raw_x = (((int16_t)buffer[8])<<8) | buffer[9];
		const int16_t gyr_raw_y = (((int16_t)buffer[10])<<8) | buffer[11];
		const int16_t gyr_raw_z = (((int16_t)buffer[12])<<8) | buffer[13];

		const float3_t gyr = {
			.x = gyr_raw_x,
			.y = gyr_raw_y,
			.z = gyr_raw_z
		};

		reading.type = SENSOR_GYROSCOPE;
		reading.data = pvPortMalloc(sizeof(float3_t));
		memcpy(reading.data, &gyr, sizeof(float3_t));
		xQueueSend(sensor_queue, &reading, 0);
	}
}

void read_mag(void *param) {
	(void)param;

	write_reg(MAG_ADDRESS, 0x00, 0x70);
	write_reg(MAG_ADDRESS, 0x01, 0xE0);
	write_reg(MAG_ADDRESS, 0x02, 0x00);

	TickType_t time = xTaskGetTickCount();

	uint8_t buffer[6];

	queue_element_t reading;

	while(1) {
		vTaskDelayUntil(&time, 100);

		read_regs(MAG_ADDRESS, 0x03, buffer, sizeof(buffer));

		const int16_t raw_x = (((int16_t)buffer[0])<<8) | buffer[1];
		const int16_t raw_y = (((int16_t)buffer[4])<<8) | buffer[5];
		const int16_t raw_z = (((int16_t)buffer[2])<<8) | buffer[3];

		const float3_t mag = {
			.x = raw_x,
			.y = raw_y,
			.z = raw_z
		};

		reading.type = SENSOR_MAGNETOMETER;
		reading.data = pvPortMalloc(sizeof(float3_t));
		memcpy(reading.data, &mag, sizeof(float3_t));
		xQueueSend(sensor_queue, &reading, 0);
	}
}

void read_bar(void *param) {
	(void)param;

	write_reg(BAR_ADDRESS, 0xE0, 0xB6);
	write_reg(BAR_ADDRESS, 0xF4, 0xFF);

	TickType_t time = xTaskGetTickCount();

	uint8_t buffer[3];

	queue_element_t reading;

	while(1) {
		vTaskDelayUntil(&time, 100);
		
		read_regs(BAR_ADDRESS, 0xF7, buffer, sizeof(buffer));

		const int32_t raw = (((int32_t)buffer[0])<<12) | (((int32_t)buffer[1])<<4) | (((int32_t)buffer[2])>>4);

		const float bar = raw;

		reading.type = SENSOR_BAROMETER;
		reading.data = pvPortMalloc(sizeof(float));
		memcpy(reading.data, &bar, sizeof(float));
		xQueueSend(sensor_queue, &reading, 0);
	}
}

void Sensors_Init() {

	bus_lock = xSemaphoreCreateBinary();
	sensor_queue = xQueueCreate(16, sizeof(queue_element_t));

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	HAL_I2C_Init(&hi2c1);

	xSemaphoreGive(bus_lock);

	xTaskCreate(read_imu, "IMU reader", 256, NULL, 4, NULL);
	xTaskCreate(read_mag, "MAG reader", 256, NULL, 4, NULL);
	xTaskCreate(read_bar, "BAR reader", 256, NULL, 4, NULL);
}
