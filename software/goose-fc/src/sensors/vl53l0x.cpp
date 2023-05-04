#include "TaskCPP.h"

#include "logger.h"
#include "transport.h"

#include "stm32f4xx_hal.h"
#include "vl53l0x_api.h"

class VL53L0X : TaskClassS<1024> {
	VL53L0X_Dev_t sensor;
	I2C_HandleTypeDef hi2c3;

	VL53L0X_RangingMeasurementData_t ranging_data;

public:
	VL53L0X();

	void gpioInit();
	void busInit();
	void sensorInit();

	bool readData();
	float getDistance() const;

	void task();
};

TaskHandle_t dist_task;

VL53L0X laser;

VL53L0X::VL53L0X() : TaskClassS{"VL53L0X reader", TaskPrio_Low} {
	sensor.I2cHandle = &hi2c3;
	sensor.I2cDevAddr = 0x52;
}

void VL53L0X::gpioInit() {
	GPIO_InitTypeDef GPIO_InitStruct;

	// interrupt
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	// xshut
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void VL53L0X::busInit() {
	hi2c3.Instance = I2C3;
	hi2c3.Init.ClockSpeed = 100000;
	hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c3.Init.OwnAddress1 = 0;
	hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c3.Init.OwnAddress2 = 0;
	hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	HAL_I2C_Init(&hi2c3);
}

void VL53L0X::sensorInit() {

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	vTaskDelay(20);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	vTaskDelay(20);

	uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;

	VL53L0X_WaitDeviceBooted(&sensor);
	VL53L0X_DataInit(&sensor);
	VL53L0X_StaticInit(&sensor);
	VL53L0X_PerformRefCalibration(&sensor, &VhvSettings, &PhaseCal);
	VL53L0X_PerformRefSpadManagement(&sensor, &refSpadCount, &isApertureSpads);
	//VL53L0X_SetDeviceMode(&sensor, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);

	VL53L0X_SetDeviceMode(&sensor, VL53L0X_DEVICEMODE_SINGLE_RANGING);

	//VL53L0X_SetLimitCheckEnable(&sensor, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
	//VL53L0X_SetLimitCheckEnable(&sensor, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
	//VL53L0X_SetLimitCheckValue(&sensor, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.1*65536));
	//VL53L0X_SetLimitCheckValue(&sensor, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(60*65536));
	VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&sensor, 33000);
	VL53L0X_SetVcselPulsePeriod(&sensor, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 18);
	VL53L0X_SetVcselPulsePeriod(&sensor, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 14);

	VL53L0X_StartMeasurement(&sensor);

	Logger::getInstance().log(Logger::INFO, "vlx: initialization complete");
}

bool VL53L0X::readData() {

	//VL53L0X_GetRangingMeasurementData(&sensor, &ranging_data);
	//VL53L0X_ClearInterruptMask(&sensor, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);

	VL53L0X_PerformSingleRangingMeasurement(&sensor, &ranging_data);

	return (ranging_data.RangeStatus==0);
}

float VL53L0X::getDistance() const {
	return ranging_data.RangeMilliMeter*0.001f;
}

void VL53L0X::task() {
	dist_task = getTaskHandle();

	gpioInit();
	busInit();
	sensorInit();

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, 33);
		//ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY);

		if(!readData()) {
			continue;
		}

		const float dist = getDistance();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::LASER, dist, 0);
	}
}
