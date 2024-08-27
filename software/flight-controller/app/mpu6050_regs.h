#ifndef MPU6050_REGS_H
#define MPU6050_REGS_H

#define MPU6050_ADDR					0x68

// registers
#define MPU6050_REG_SELF_TEST_X			0x0D
#define MPU6050_REG_SELF_TEST_Y			0x0E
#define MPU6050_REG_SELF_TEST_Z			0x0F
#define MPU6050_REG_SELF_TEST_A			0x10

#define MPU6050_REG_SMPLRT_DIV			0x19
#define MPU6050_REG_CONFIG				0x1A
#define MPU6050_REG_GYRO_CONFIG			0x1B
#define MPU6050_REG_ACCEL_CONFIG		0x1C
#define MPU6050_REG_FIFO_EN				0x23

#define MPU6050_REG_I2C_MST_CTRL		0x24
#define MPU6050_REG_I2C_SLV0_ADDR		0x25
#define MPU6050_REG_I2C_SLV0_REG		0x26
#define MPU6050_REG_I2C_SLV0_CTRL		0x27
#define MPU6050_REG_I2C_SLV1_ADDR		0x28
#define MPU6050_REG_I2C_SLV1_REG		0x29
#define MPU6050_REG_I2C_SLV1_CTRL		0x2A
#define MPU6050_REG_I2C_SLV2_ADDR		0x2B
#define MPU6050_REG_I2C_SLV2_REG		0x2C
#define MPU6050_REG_I2C_SLV2_CTRL		0x2D
#define MPU6050_REG_I2C_SLV3_ADDR		0x2E
#define MPU6050_REG_I2C_SLV3_REG		0x2F
#define MPU6050_REG_I2C_SLV3_CTRL		0x30
#define MPU6050_REG_I2C_SLV4_ADDR		0x31
#define MPU6050_REG_I2C_SLV4_REG		0x32
#define MPU6050_REG_I2C_SLV4_DO			0x33
#define MPU6050_REG_I2C_SLV4_CTRL		0x34
#define MPU6050_REG_I2C_SLV4_DI			0x35
#define MPU6050_REG_I2C_MST_STATUS		0x36

#define MPU6050_REG_INT_PIN_CFG			0x37
#define MPU6050_REG_INT_ENABLE			0x38
#define MPU6050_REG_INT_STATUS			0x3A

#define MPU6050_REG_ACCEL_XOUT_H		0x3B
#define MPU6050_REG_ACCEL_XOUT_L		0x3C
#define MPU6050_REG_ACCEL_YOUT_H		0x3D
#define MPU6050_REG_ACCEL_YOUT_L		0x3E
#define MPU6050_REG_ACCEL_ZOUT_H		0x3F
#define MPU6050_REG_ACCEL_ZOUT_L		0x40
#define MPU6050_REG_TEMP_OUT_H			0x41
#define MPU6050_REG_TEMP_OUT_L			0x42
#define MPU6050_REG_GYRO_XOUT_H			0x43
#define MPU6050_REG_GYRO_XOUT_L			0x44
#define MPU6050_REG_GYRO_YOUT_H			0x45
#define MPU6050_REG_GYRO_YOUT_L			0x46
#define MPU6050_REG_GYRO_ZOUT_H			0x47
#define MPU6050_REG_GYRO_ZOUT_L			0x48

#define MPU6050_REG_EXT_SENS_DATA_00	0x49
#define MPU6050_REG_EXT_SENS_DATA_01	0x4A
#define MPU6050_REG_EXT_SENS_DATA_02	0x4B
#define MPU6050_REG_EXT_SENS_DATA_03	0x4C
#define MPU6050_REG_EXT_SENS_DATA_04	0x4D
#define MPU6050_REG_EXT_SENS_DATA_05	0x4E
#define MPU6050_REG_EXT_SENS_DATA_06	0x4F
#define MPU6050_REG_EXT_SENS_DATA_07	0x50
#define MPU6050_REG_EXT_SENS_DATA_08	0x51
#define MPU6050_REG_EXT_SENS_DATA_09	0x52
#define MPU6050_REG_EXT_SENS_DATA_10	0x53
#define MPU6050_REG_EXT_SENS_DATA_11	0x54
#define MPU6050_REG_EXT_SENS_DATA_12	0x55
#define MPU6050_REG_EXT_SENS_DATA_13	0x56
#define MPU6050_REG_EXT_SENS_DATA_14	0x57
#define MPU6050_REG_EXT_SENS_DATA_15	0x58
#define MPU6050_REG_EXT_SENS_DATA_16	0x59
#define MPU6050_REG_EXT_SENS_DATA_17	0x5A
#define MPU6050_REG_EXT_SENS_DATA_18	0x5B
#define MPU6050_REG_EXT_SENS_DATA_19	0x5C
#define MPU6050_REG_EXT_SENS_DATA_20	0x5D
#define MPU6050_REG_EXT_SENS_DATA_21	0x5E
#define MPU6050_REG_EXT_SENS_DATA_22	0x5F
#define MPU6050_REG_EXT_SENS_DATA_23	0x60

#define MPU6050_REG_I2C_SLV0_DO			0x63
#define MPU6050_REG_I2C_SLV1_DO			0x64
#define MPU6050_REG_I2C_SLV2_DO			0x65
#define MPU6050_REG_I2C_SLV3_DO			0x66
#define MPU6050_REG_I2C_MST_DELAY_CTRL	0x67

#define MPU6050_REG_SIGNAL_PATH_RESET	0x68
#define MPU6050_REG_USER_CTRL			0x6A
#define MPU6050_REG_PWR_MGMT_1			0x6B
#define MPU6050_REG_PWR_MGMT_2			0x6C
#define MPU6050_REG_FIFO_COUNTH			0x72
#define MPU6050_REG_FIFO_COUNTL			0x73
#define MPU6050_REG_FIFO_R_W			0x74
#define MPU6050_REG_WHO_AM_I			0x75

// bits
#define MPU6050_CONFIG_EXT_SYNC_DISABLED			(0x00<<3)
#define MPU6050_CONFIG_EXT_SYNC_TEMP_OUT_L			(0x01<<3)
#define MPU6050_CONFIG_EXT_SYNC_GYRO_XOUT_L			(0x02<<3)
#define MPU6050_CONFIG_EXT_SYNC_GYRO_YOUT_L			(0x03<<3)
#define MPU6050_CONFIG_EXT_SYNC_GYRO_ZOUT_L			(0x04<<3)
#define MPU6050_CONFIG_EXT_SYNC_ACCEL_XOUT_L		(0x05<<3)
#define MPU6050_CONFIG_EXT_SYNC_ACCEL_YOUT_L		(0x06<<3)
#define MPU6050_CONFIG_EXT_SYNC_ACCEL_ZOUT_L		(0x07<<3)
#define MPU6050_CONFIG_DLPF_SETTING_0				(0x00<<0)
#define MPU6050_CONFIG_DLPF_SETTING_1				(0x01<<0)
#define MPU6050_CONFIG_DLPF_SETTING_2				(0x02<<0)
#define MPU6050_CONFIG_DLPF_SETTING_3				(0x03<<0)
#define MPU6050_CONFIG_DLPF_SETTING_4				(0x04<<0)
#define MPU6050_CONFIG_DLPF_SETTING_5				(0x05<<0)
#define MPU6050_CONFIG_DLPF_SETTING_6				(0x06<<0)

#define MPU6050_GYRO_CONFIG_X_SELF_TEST				(0x01<<7)
#define MPU6050_GYRO_CONFIG_Y_SELF_TEST				(0x01<<6)
#define MPU6050_GYRO_CONFIG_Z_SELF_TEST				(0x01<<5)
#define MPU6050_GYRO_CONFIG_RANGE_250DPS			(0x00<<3)
#define MPU6050_GYRO_CONFIG_RANGE_500DPS			(0x01<<3)
#define MPU6050_GYRO_CONFIG_RANGE_1000DPS			(0x02<<3)
#define MPU6050_GYRO_CONFIG_RANGE_2000DPS			(0x03<<3)

#define MPU6050_ACCEL_CONFIG_X_SELF_TEST			(0x01<<7)
#define MPU6050_ACCEL_CONFIG_Y_SELF_TEST			(0x01<<6)
#define MPU6050_ACCEL_CONFIG_Z_SELF_TEST			(0x01<<5)
#define MPU6050_ACCEL_CONFIG_RANGE_2G				(0x00<<3)
#define MPU6050_ACCEL_CONFIG_RANGE_4G				(0x01<<3)
#define MPU6050_ACCEL_CONFIG_RANGE_8G				(0x02<<3)
#define MPU6050_ACCEL_CONFIG_RANGE_16G				(0x03<<3)

#define MPU6050_FIFO_EN_TEMP						(0x01<<7)
#define MPU6050_FIFO_EN_XG							(0x01<<6)
#define MPU6050_FIFO_EN_YG							(0x01<<5)
#define MPU6050_FIFO_EN_ZG							(0x01<<4)
#define MPU6050_FIFO_EN_ACCEL						(0x01<<3)
#define MPU6050_FIFO_EN_SLV2						(0x01<<2)
#define MPU6050_FIFO_EN_SLV1						(0x01<<1)
#define MPU6050_FIFO_EN_SLV0						(0x01<<0)

#define MPU6050_INT_PIN_CFG_LEVEL_ACTIVE_HIGH		(0x00<<7)
#define MPU6050_INT_PIN_CFG_LEVEL_ACTIVE_LOW		(0x01<<7)
#define MPU6050_INT_PIN_CFG_PUSH_PULL				(0x00<<6)
#define MPU6050_INT_PIN_CFG_OPEN_DRAIN				(0x01<<6)
#define MPU6050_INT_PIN_CFG_PULSE					(0X00<<5)
#define MPU6050_INT_PIN_CFG_LATCH_PIN				(0X01<<5)
#define MPU6050_INT_PIN_CFG_STATUS_CLEAR_AFTER_READ	(0X00<<4)
#define MPU6050_INT_PIN_CFG_STATUS_CLEAR_AFTER_ANY	(0X01<<4)
#define MPU6050_INT_PIN_CFG_FSYNC_LEVEL_ACTIVE_HIGH	(0X00<<3)
#define MPU6050_INT_PIN_CFG_FSYNC_LEVEL_ACTIVE_LOW	(0X01<<3)
#define MPU6050_INT_PIN_CFG_FSYNC_DISABLE			(0X00<<2)
#define MPU6050_INT_PIN_CFG_FSYNC_ENABLE			(0X01<<2)
#define MPU6050_INT_PIN_CFG_I2C_BYPASS_DISABLE		(0X00<<1)
#define MPU6050_INT_PIN_CFG_I2C_BYPASS_ENABLE		(0X01<<1)

#define MPU6050_INT_ENABLE_FIFO_OVERLOW_DISABLE		(0x00<<4)
#define MPU6050_INT_ENABLE_FIFO_OVERLOW_ENABLE		(0x01<<4)
#define MPU6050_INT_ENABLE_I2C_MST_INT_DISABLE		(0x00<<3)
#define MPU6050_INT_ENABLE_I2C_MST_INT_ENABLE		(0x01<<3)
#define MPU6050_INT_ENABLE_DATA_RDY_DISABLE			(0x00<<0)
#define MPU6050_INT_ENABLE_DATA_RDY_ENABLE			(0x01<<0)

#define MPU6050_SIGNAL_PATH_RESET_GYRO				(0x01<<2)
#define MPU6050_SIGNAL_PATH_RESET_ACCEL				(0x01<<1)
#define MPU6050_SIGNAL_PATH_RESET_TEMP				(0x01<<0)

#define MPU6050_USER_CTRL_FIFO_EN					(0x01<<6)
#define MPU6050_USER_CTRL_I2C_MST_EN				(0x01<<5)
#define MPU6050_USER_CTRL_I2C_IF_DIS				(0x01<<4)
#define MPU6050_USER_CTRL_FIFO_RESET				(0x01<<2)
#define MPU6050_USER_CTRL_I2C_MST_RESET				(0x01<<1)
#define MPU6050_USER_CTRL_SIG_COND_RESET			(0x01<<0)

#define MPU6050_PWR_MGMT_1_DEVICE_RESET				(0x01<<7)
#define MPU6050_PWR_MGMT_1_SLEEP					(0x01<<6)
#define MPU6050_PWR_MGMT_1_CYCLE					(0x01<<5)
#define MPU6050_PWR_MGMT_1_TEMP_DIS					(0x01<<3)
#define MPU6050_PWR_MGMT_1_CLOCK_INTERNAL			(0x00<<0)
#define MPU6050_PWR_MGMT_1_CLOCK_GYRO_X				(0x01<<0)
#define MPU6050_PWR_MGMT_1_CLOCK_GYRO_Y				(0x02<<0)
#define MPU6050_PWR_MGMT_1_CLOCK_GYRO_Z				(0x03<<0)
#define MPU6050_PWR_MGMT_1_CLOCK_EXTERNAL_32_768KHZ	(0x04<<0)
#define MPU6050_PWR_MGMT_1_CLOCK_EXTERNAL_19_2MHZ	(0x05<<0)
#define MPU6050_PWR_MGMT_1_CLOCK_STOP				(0x07<<0)

#define MPU6050_PWR_MGMT_2_LP_WAKE_1_25HZ			(0x00<<6)
#define MPU6050_PWR_MGMT_2_LP_WAKE_5HZ				(0x01<<6)
#define MPU6050_PWR_MGMT_2_LP_WAKE_20HZ				(0x02<<6)
#define MPU6050_PWR_MGMT_2_LP_WAKE_40HZ				(0x03<<6)
#define MPU6050_PWR_MGMT_2_STBY_XA					(0x01<<5)
#define MPU6050_PWR_MGMT_2_STBY_YA					(0x01<<4)
#define MPU6050_PWR_MGMT_2_STBY_ZA					(0x01<<3)
#define MPU6050_PWR_MGMT_2_STBY_XG					(0x01<<2)
#define MPU6050_PWR_MGMT_2_STBY_YG					(0x01<<1)
#define MPU6050_PWR_MGMT_2_STBY_ZG					(0x01<<0)

#define MPU6050_WHO_AM_I_VALUE						(0x68<<0)

#endif
/*
class MPU6050 : public TaskClassS<512> {
	IntervalLogger<Vector> telemetry_acc;
	IntervalLogger<Vector> telemetry_gyr;

	uint8_t buffer[14];

public:

	MPU6050();

	void init();

	bool readData();
	Vector getGyration() const;
	Vector getAcceleration() const;

	void task();
};

TaskHandle_t imu_task;

MPU6050 imu;

MPU6050::MPU6050() : TaskClassS{"MPU6050 reader", TaskPrio_Low},
		telemetry_acc{"MPU6050 telemetry acc", Transfer::ID::SENSOR_ACCELERATION},
		telemetry_gyr{"MPU6050 telemetry gyr", Transfer::ID::SENSOR_GYRATION} {

}

void MPU6050::init() {

	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	SensorBus::getInstance().write(MPU6050_ADDR, MPU6050_REG_PWR_MGMT_1, static_cast<uint8_t>(
		MPU6050_PWR_MGMT_1_DEVICE_RESET
	));

	vTaskDelay(100);

	SensorBus::getInstance().write(MPU6050_ADDR, MPU6050_REG_SIGNAL_PATH_RESET, static_cast<uint8_t>(
		MPU6050_SIGNAL_PATH_RESET_GYRO |
		MPU6050_SIGNAL_PATH_RESET_ACCEL |
		MPU6050_SIGNAL_PATH_RESET_TEMP
	));

	vTaskDelay(100);

	SensorBus::getInstance().write(MPU6050_ADDR, MPU6050_REG_INT_ENABLE, static_cast<uint8_t>(
		MPU6050_INT_ENABLE_FIFO_OVERLOW_DISABLE |
		MPU6050_INT_ENABLE_I2C_MST_INT_DISABLE |
		MPU6050_INT_ENABLE_DATA_RDY_ENABLE
	));

	SensorBus::getInstance().write(MPU6050_ADDR, MPU6050_REG_INT_PIN_CFG, static_cast<uint8_t>(
		MPU6050_INT_PIN_CFG_LEVEL_ACTIVE_HIGH |
		MPU6050_INT_PIN_CFG_PUSH_PULL |
		MPU6050_INT_PIN_CFG_PULSE |
		MPU6050_INT_PIN_CFG_STATUS_CLEAR_AFTER_ANY |
		MPU6050_INT_PIN_CFG_FSYNC_DISABLE |
		MPU6050_INT_PIN_CFG_I2C_BYPASS_DISABLE
	));

	SensorBus::getInstance().write(MPU6050_ADDR, MPU6050_REG_PWR_MGMT_1, static_cast<uint8_t>(
		MPU6050_PWR_MGMT_1_TEMP_DIS |
		MPU6050_PWR_MGMT_1_CLOCK_INTERNAL
	));

	SensorBus::getInstance().write(MPU6050_ADDR, MPU6050_REG_CONFIG, static_cast<uint8_t>(
		MPU6050_CONFIG_EXT_SYNC_DISABLED |
		MPU6050_CONFIG_DLPF_SETTING_6
	));

	SensorBus::getInstance().write(MPU6050_ADDR, MPU6050_REG_ACCEL_CONFIG, static_cast<uint8_t>(
		MPU6050_ACCEL_CONFIG_RANGE_4G
	));

	SensorBus::getInstance().write(MPU6050_ADDR, MPU6050_REG_GYRO_CONFIG, static_cast<uint8_t>(
		MPU6050_GYRO_CONFIG_RANGE_500DPS
	));

	SensorBus::getInstance().write(MPU6050_ADDR, MPU6050_REG_SMPLRT_DIV, static_cast<uint8_t>(4));

	Logger::getInstance().log(Logger::INFO, "imu: initialization complete");
}

bool MPU6050::readData() {

	if(SensorBus::getInstance().read(MPU6050_ADDR, MPU6050_REG_ACCEL_XOUT_H, buffer, sizeof(buffer))) {
		return false;
	}

	return true;
}

Vector MPU6050::getGyration() const {
	const int16_t raw_x = (((int16_t)buffer[8])<<8) | buffer[9];
	const int16_t raw_y = (((int16_t)buffer[10])<<8) | buffer[11];
	const int16_t raw_z = (((int16_t)buffer[12])<<8) | buffer[13];

	constexpr float gain = 65.5f;
	constexpr float dps_to_rads = 0.017453292519943f;

	const Vector gyration = Vector(raw_z, raw_x, raw_y)*dps_to_rads/gain;

	return gyration;
}

Vector MPU6050::getAcceleration() const {
	const int16_t raw_x = (((int16_t)buffer[0])<<8) | buffer[1];
	const int16_t raw_y = (((int16_t)buffer[2])<<8) | buffer[3];
	const int16_t raw_z = (((int16_t)buffer[4])<<8) | buffer[5];

	constexpr float gain = 8192.f;
	constexpr float g_to_ms2 = 9.81f;

	const Vector pre_calibrated = Vector(raw_z, raw_x, raw_y)*g_to_ms2/gain;

	constexpr Vector offset = {-0.3485f, 0.2071f, -0.0096f};
	constexpr Matrix<3, 3> scale = {
		0.9866f,	0,			0,
		0,			0.9919f,	0,
		0,			0,			0.9948f
	};

	const Vector acceleration = scale*(pre_calibrated - offset);

	return acceleration;
}

void MPU6050::task() {
	imu_task = getTaskHandle();

	init();

	while(1) {
		ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY);

		if(!readData()) {
			continue;
		}

		const Vector acc = getAcceleration();
		const Vector gyr = getGyration();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::ACCELEROMETER, acc, 0);
		Transport::getInstance().sensor_queue.add(Transport::Sensors::GYROSCOPE, gyr, 0);

		telemetry_acc.feed(acc);
		telemetry_gyr.feed(gyr);
	}
}
*/