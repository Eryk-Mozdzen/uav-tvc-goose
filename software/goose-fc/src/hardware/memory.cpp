#include "memory.h"
#include <stm32f4xx_hal.h>
#include "logger.h"

Memory::Memory() : lock{"flash lock"} {

}

void Memory::synchronize() {
	const comm::Memory mem = read();

	memcpy(&copy, &mem, sizeof(comm::Memory));
}

void Memory::write(const comm::Memory &data) {
	const uint8_t *src = reinterpret_cast<const uint8_t *>(&data);
	const uint32_t bytes = sizeof(comm::Memory);

	FLASH_EraseInitTypeDef erase;
	erase.TypeErase = FLASH_TYPEERASE_SECTORS;
	erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	erase.Sector = FLASH_SECTOR_7;
	erase.NbSectors = 1;

	lock.take(portMAX_DELAY);
	HAL_FLASH_Unlock();

	uint32_t error;
	if(HAL_FLASHEx_Erase(&erase, &error)!=HAL_OK) {
		Logger::getInstance().log(Logger::ERROR, "mem: flash erase error");
		HAL_FLASH_Lock();
		lock.give();
		return;
	}

	for(uint32_t i=0; i<bytes; i++) {
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, sector_address + i, src[i])!=HAL_OK) {
			Logger::getInstance().log(Logger::ERROR, "mem: flash write error");
			HAL_FLASH_Lock();
			lock.give();
			return;
		}
	}

	HAL_FLASH_Lock();
	lock.give();

	memcpy(&copy, &data, bytes);

	Logger::getInstance().log(Logger::DEBUG, "mem: successfully write %u bytes to 0x%08X in flash memory", bytes, sector_address);
}

comm::Memory Memory::read() {
	comm::Memory memory;

	const uint8_t *src = reinterpret_cast<const uint8_t *>(sector_address);
	const uint32_t bytes = sizeof(comm::Memory);

	lock.take(portMAX_DELAY);
	memcpy(&memory, src, bytes);
	lock.give();

	Logger::getInstance().log(Logger::DEBUG, "mem: successfully read %u bytes from 0x%08X in flash memory", sizeof(comm::Memory), sector_address);

	return memory;
}

comm::Memory Memory::get() const {
	return copy;
}

Memory & Memory::getInstance() {
    static Memory instance;

    return instance;
}
