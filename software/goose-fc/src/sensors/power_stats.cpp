#include "TaskCPP.h"

#include "logger.h"
#include "transport.h"
#include "interval_logger.h"

#include "stm32f4xx_hal.h"

class PowerStats : TaskClassS<512> {

	struct Analog {
		static constexpr float reference = 3.3f;
		static constexpr uint8_t resolution = 12;
		static constexpr float adc2volt = reference/(1<<resolution);
	};

	struct SupplyVoltage {
		static constexpr float R11 = 470000.f;
		static constexpr float R12 = 100000.f;
		static constexpr float gain = R12/(R11+R12);
	};

	struct SupplyCurrent {
		static constexpr float R33 = 4700.f;
		static constexpr float R34 = 210000.f;
		static constexpr float Rshunt = 0.001f;
		static constexpr float gain = (R34/R33)*Rshunt;
	};

	ADC_HandleTypeDef hadc1;

	IntervalLogger<float> telemetry_voltage;
	IntervalLogger<float> telemetry_current;

	uint16_t readings[2];

	void adcInit();

public:
	PowerStats();

	float getSupplyVoltage() const;
	float getSupplyCurrent() const;

	void task();
};

DMA_HandleTypeDef hdma_adc1;
PowerStats power_stats;

PowerStats::PowerStats() : TaskClassS{"Power Stats reader", TaskPrio_Low},
		telemetry_voltage{"Power Stats telemetry voltage", Transfer::ID::TELEMETRY_SENSOR_VOLTAGE},
		telemetry_current{"Power Stats telemetry current", Transfer::ID::TELEMETRY_SENSOR_CURRENT} {

}

void PowerStats::adcInit() {
	__HAL_RCC_DMA2_CLK_ENABLE();

	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = ENABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 2;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	HAL_ADC_Init(&hadc1);

	ADC_ChannelConfTypeDef sConfig;

	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = 2;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)readings, 2);
}

float PowerStats::getSupplyVoltage() const {
	const float voltage = readings[0]*Analog::adc2volt;

	return voltage/SupplyVoltage::gain;
}

float PowerStats::getSupplyCurrent() const {
	const float voltage = readings[1]*Analog::adc2volt;

	return voltage/SupplyCurrent::gain;
}

void PowerStats::task() {
	adcInit();

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, 20);

		const float voltage = getSupplyVoltage();
		const float current = getSupplyCurrent();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::VOLTAGE, voltage, 0);
		Transport::getInstance().sensor_queue.add(Transport::Sensors::CURRENT, current, 0);

		telemetry_voltage.feed(voltage);
		telemetry_current.feed(current);
	}
}
