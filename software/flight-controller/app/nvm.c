#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "stm32u5xx_hal.h"

#define BEGIN 0x0807E000

void nvm_read(const uint32_t address, void *dest, const size_t len) {
    memcpy(dest, (void *)(BEGIN + address), len);
}

void nvm_write(const uint32_t address, const void *src, const size_t len) {
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = FLASH_BANK_2,
        .Page = 31,
        .NbPages = 1,
    };

    uint32_t error;
    HAL_FLASHEx_Erase(&erase, &error);

    uint32_t addr = address;
    uint32_t *data = (uint32_t *)src;
    uint32_t remaining = len;

    while(remaining>=16) {
        uint32_t quadword[4] = {0};
        memcpy(quadword, data, 16);

        HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, BEGIN + addr, (uint32_t)quadword);

        addr +=16;
        data +=4;
        remaining -=16;
    }

    if(remaining>0) {
        uint32_t quadword[4] = {0};
        memcpy(quadword, data, remaining);

        HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, BEGIN + addr, (uint32_t)quadword);
    }

    HAL_FLASH_Lock();
}
