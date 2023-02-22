#include "imu9dof.h"
#include "task.h"
#include "lsm303dlhc.h"
#include "l3gd20.h"

IMU_9DoF::IMU_9DoF(I2C_HandleTypeDef &hi2c) : hi2c{hi2c} {
	lock = xSemaphoreCreateBinary();

	xSemaphoreGive(lock);
}

IMU_9DoF::~IMU_9DoF() {
	vSemaphoreDelete(lock);
}

bool IMU_9DoF::initializeSensors() {
	
	{
		LSM303DLHC::CTRL_REG1_A reg;
		reg.bits = 0;
		reg.odr = LSM303DLHC::CTRL_REG1_A::ODR::NORMAL_LOW_POWER_200HZ;
		reg.lpen = LSM303DLHC::CTRL_REG1_A::LPen::NORMAL_MODE;
		reg.xen = LSM303DLHC::CTRL_REG1_A::Xen::X_AXIS_ENABLED;
		reg.yen = LSM303DLHC::CTRL_REG1_A::Yen::Y_AXIS_ENABLED;
		reg.zen = LSM303DLHC::CTRL_REG1_A::Zen::Z_AXIS_ENABLED;
		HAL_I2C_Mem_Write(&hi2c, LSM303DLHC::acc_address, LSM303DLHC::CTRL_REG1_A::address, 1, &reg.bits, 1, HAL_MAX_DELAY);
	}

	{
		LSM303DLHC::CTRL_REG4_A reg;
		reg.bits = 0;
		reg.bdu = LSM303DLHC::CTRL_REG4_A::BDU::OUTPUT_REGISTERS_NOT_UPDATED_UNTIL_MSB_AND_LSB_HAVE_BEEN_READ;
		reg.ble = LSM303DLHC::CTRL_REG4_A::BLE::DATA_LSB_AT_LOWER_ADDRESS;
		reg.fs = LSM303DLHC::CTRL_REG4_A::FS::_16G;
		reg.hr = LSM303DLHC::CTRL_REG4_A::HR::HIGH_RESOLUTION_ENABLED;
		HAL_I2C_Mem_Write(&hi2c, LSM303DLHC::acc_address, LSM303DLHC::CTRL_REG4_A::address, 1, &reg.bits, 1, HAL_MAX_DELAY);
	}

	{
		LSM303DLHC::CRA_REG_M reg;
		reg.bits = 0;
		reg.temp_en = LSM303DLHC::CRA_REG_M::TEMP_EN::TEMPERATURE_SENSOR_DISABLED;
		reg._do = LSM303DLHC::CRA_REG_M::DO::DATA_OUTPUT_RATE_220HZ;
		HAL_I2C_Mem_Write(&hi2c, LSM303DLHC::mag_address, LSM303DLHC::CRA_REG_M::address, 1, &reg.bits, 1, HAL_MAX_DELAY);
	}

	{
		LSM303DLHC::CRB_REG_M reg;
		reg.bits = 0;
		reg.gn = LSM303DLHC::CRB_REG_M::GN::SENSOR_INPUT_FIELD_RANGE_1_3GAUSS;
		HAL_I2C_Mem_Write(&hi2c, LSM303DLHC::mag_address, LSM303DLHC::CRB_REG_M::address, 1, &reg.bits, 1, HAL_MAX_DELAY);
	}

	{
		LSM303DLHC::MR_REG_M reg;
		reg.bits = 0;
		reg.md = LSM303DLHC::MR_REG_M::MD::CONTINOUS_CONVERSION_MODE;
		HAL_I2C_Mem_Write(&hi2c, LSM303DLHC::mag_address, LSM303DLHC::MR_REG_M::address, 1, &reg.bits, 1, HAL_MAX_DELAY);
	}

	{
		L3GD20::CTRL_REG1 reg;
		reg.bits = 0;
		reg.dr_bw = L3GD20::CTRL_REG1::DR_BW::OUTPUT_DATA_RATE_190HZ_CUTOFF_12_5;
		reg.pd = L3GD20::CTRL_REG1::PD::NORMAL_MODE_OR_SLEEP_MODE;
		reg.xen = L3GD20::CTRL_REG1::Xen::X_AXIS_ENABLED;
		reg.yen = L3GD20::CTRL_REG1::Yen::Y_AXIS_ENABLED;
		reg.zen = L3GD20::CTRL_REG1::Zen::Z_AXIS_ENABLED;
		HAL_I2C_Mem_Write(&hi2c, L3GD20::address, L3GD20::CTRL_REG1::address, 1, &reg.bits, 1, HAL_MAX_DELAY);
	}

	{
		L3GD20::CTRL_REG4 reg;
		reg.bits = 0;
		reg.bdu = L3GD20::CTRL_REG4::BDU::OUTPUT_REGISTERS_NOT_UPDATED_UNTIL_MSB_AND_LSB_HAVE_BEEN_READ;
		reg.ble = L3GD20::CTRL_REG4::BLE::DATA_LSB_AT_LOWER_ADDRESS;
		reg.fs = L3GD20::CTRL_REG4::FS::_250DPS;
		HAL_I2C_Mem_Write(&hi2c, L3GD20::address, L3GD20::CTRL_REG4::address, 1, &reg.bits, 1, HAL_MAX_DELAY);
	}

	return true;
}

void IMU_9DoF::startSampling() {
	xTaskCreate(&IMU_9DoF::task, "IMU read", 1024, this, 4, nullptr);
}

void IMU_9DoF::task(void *param) {
	static_cast<IMU_9DoF *>(param)->read();
}

void IMU_9DoF::read() {

	uint8_t acc[6];
	uint8_t mag[6];
	uint8_t gyr[6];

	while(true) {
		
		HAL_I2C_Mem_Read(&hi2c, LSM303DLHC::acc_address, LSM303DLHC::OUT_X_A::address, 1, acc, 6, HAL_MAX_DELAY);
		HAL_I2C_Mem_Read(&hi2c, LSM303DLHC::mag_address, LSM303DLHC::OUT_X_M::address, 1, mag, 6, HAL_MAX_DELAY);
		HAL_I2C_Mem_Read(&hi2c, L3GD20::address,		 L3GD20::OUT_X::address,       1, gyr, 6, HAL_MAX_DELAY);

		xSemaphoreTake(lock, portMAX_DELAY);

		acceleration.x = acc[1]<<8 | acc[0];
		acceleration.y = acc[3]<<8 | acc[2];
		acceleration.z = acc[5]<<8 | acc[4];
		
		magnetic_field.x = mag[0]<<8 | mag[1];
		magnetic_field.y = mag[4]<<8 | mag[5];
		magnetic_field.z = mag[2]<<8 | mag[3];

		gyration.x = gyr[1]<<8 | gyr[0];
		gyration.y = gyr[3]<<8 | gyr[2];
		gyration.z = gyr[5]<<8 | gyr[4];

		xSemaphoreGive(lock);

		vTaskDelay(100);
	}
}

IMU_9DoF::Vector3i IMU_9DoF::getRawAcceleration() {
	xSemaphoreTake(lock, portMAX_DELAY);

	const Vector3i value = acceleration;

	xSemaphoreGive(lock);

	return value;
}

IMU_9DoF::Vector3i IMU_9DoF::getRawGyration() {
	xSemaphoreTake(lock, portMAX_DELAY);

	const Vector3i value = gyration;

	xSemaphoreGive(lock);

	return value;
}

IMU_9DoF::Vector3i IMU_9DoF::getRawMagneticField() {
	xSemaphoreTake(lock, portMAX_DELAY);

	const Vector3i value = magnetic_field;

	xSemaphoreGive(lock);

	return value;
}

IMU_9DoF::Vector3f IMU_9DoF::getAcceleration() {
	const Vector3i data = getRawAcceleration();

	Vector3f result;
	result.x = Accel::scale*(data.x - Accel::offset.x);
	result.y = Accel::scale*(data.y - Accel::offset.y);
	result.z = Accel::scale*(data.z - Accel::offset.z);

	return result;
}

IMU_9DoF::Vector3f IMU_9DoF::getGyration() {
	const Vector3i data = getRawGyration();

	Vector3f result;
	result.x = Gyro::scale*(data.x - Gyro::offset.x);
	result.y = Gyro::scale*(data.y - Gyro::offset.y);
	result.z = Gyro::scale*(data.z - Gyro::offset.z);

	return result;
}

IMU_9DoF::Vector3f IMU_9DoF::getMagneticField() {
	const Vector3i data = getRawMagneticField();

	Vector3f result;
	result.x = Mag::scale.x*(data.x - Mag::offset.x);
	result.y = Mag::scale.y*(data.y - Mag::offset.y);
	result.z = Mag::scale.z*(data.z - Mag::offset.z);

	return result;
}
