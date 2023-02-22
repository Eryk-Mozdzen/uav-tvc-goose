#ifndef SENSOR_BUS
#define SENSOR_BUS

#include <stdint.h>

void SensorBus_Init();

void SensorBus_WriteReg(const uint8_t device, const uint8_t reg, uint8_t src);
uint8_t SensorBus_ReadRegs(const uint8_t device, const uint8_t reg, uint8_t *dest, const uint8_t len);

#endif
