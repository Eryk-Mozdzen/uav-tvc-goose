#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "stm32u5xx_hal.h"

#define PAGE_BEGIN 0x0807E000
#define PAGE_SIZE  0x2000

void nvm_read(const uint32_t address, void *dest, const size_t len) {
    memcpy(dest, (void *)(PAGE_BEGIN + address), len);
}

void nvm_write(const uint32_t address, const void *src, const size_t len) {
    uint8_t page[PAGE_SIZE];
    memcpy(page, (void *)PAGE_BEGIN, PAGE_SIZE);
    memcpy((void *)(page + address), src, len);

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = FLASH_BANK_2,
        .Page = 31,
        .NbPages = 1,
    };

    uint32_t error;
    HAL_FLASHEx_Erase(&erase, &error);

    for(uint32_t i=0; i<PAGE_SIZE; i +=16) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, PAGE_BEGIN + i, (uint32_t)(page + i));
    }

    HAL_FLASH_Lock();
}
