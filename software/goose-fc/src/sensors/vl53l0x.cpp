#include "TaskCPP.h"
#include "TimerCPP.h"

#include "logger.h"
#include "transport.h"

#include "stm32f4xx_hal.h"
#include "vl53l0x_api.h"

class VL53L0X : TaskClassS<1024> {
	I2C_HandleTypeDef hi2c3;
	VL53L0X_Dev_t vlx_sensor;
	VL53L0X_RangingMeasurementData_t vlx_ranging_data;

	TimerMember<VL53L0X> data_timer;

	void dataTimeout();

	void gpioInit();
	void busInit();
	void sensorInit();

public:
	VL53L0X();

	bool readData();
	float getDistance() const;

	void task();
};

TaskHandle_t vlx_task;

VL53L0X laser;

VL53L0X::VL53L0X() : TaskClassS{"VL53L0X reader", TaskPrio_Low},
		data_timer{"VLX data timeout", this, &VL53L0X::dataTimeout, 1000, pdFALSE} {
	vlx_sensor.I2cHandle = &hi2c3;
	vlx_sensor.I2cDevAddr = 0x52;
}

void VL53L0X::gpioInit() {
	GPIO_InitTypeDef GPIO_InitStruct;

	// interrupt
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	// xshut
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
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

	VL53L0X_Error status = VL53L0X_ERROR_NONE;

	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

	VL53L0X_WaitDeviceBooted(&vlx_sensor);
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_DataInit(&vlx_sensor);
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_StaticInit(&vlx_sensor);
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_PerformRefCalibration(&vlx_sensor, &VhvSettings, &PhaseCal);
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_PerformRefSpadManagement(&vlx_sensor, &refSpadCount, &isApertureSpads);
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_SetDeviceMode(&vlx_sensor, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);

	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_SetLimitCheckEnable(&vlx_sensor, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_SetLimitCheckEnable(&vlx_sensor, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_SetLimitCheckValue(&vlx_sensor, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.1*65536));
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_SetLimitCheckValue(&vlx_sensor, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(60*65536));
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&vlx_sensor, 33000);
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_SetVcselPulsePeriod(&vlx_sensor, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 18);
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_SetVcselPulsePeriod(&vlx_sensor, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 14);

	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_StartMeasurement(&vlx_sensor);

	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	if(status!=VL53L0X_ERROR_NONE) {
		Logger::getInstance().log(Logger::ERROR, "vlx: initialization error: %d", status);
		return;
	}

	Logger::getInstance().log(Logger::INFO, "vlx: initialization complete");
}

void VL53L0X::dataTimeout() {
	Logger::getInstance().log(Logger::WARNING, "vlx: data timeout");
}

bool VL53L0X::readData() {
	VL53L0X_Error status = VL53L0X_ERROR_NONE;

	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_GetRangingMeasurementData(&vlx_sensor, &vlx_ranging_data);
	if(status==VL53L0X_ERROR_NONE) status = VL53L0X_ClearInterruptMask(&vlx_sensor, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);

	return (status==VL53L0X_ERROR_NONE && vlx_ranging_data.RangeStatus==0);
}

float VL53L0X::getDistance() const {
	return vlx_ranging_data.RangeMilliMeter*0.001f;
}

void VL53L0X::task() {
	vlx_task = getTaskHandle();

	gpioInit();
	busInit();
	sensorInit();

	data_timer.start();

	while(1) {

		ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY);

		if(!readData()) {
			continue;
		}

		data_timer.start();

		const float dist = getDistance();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::LASER, dist, 0);
	}
}
