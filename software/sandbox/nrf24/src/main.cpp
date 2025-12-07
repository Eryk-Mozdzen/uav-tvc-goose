#include <FreeRTOS.h>
#include <stm32u0xx_hal.h>
#include <task.h>

extern "C" void SystemClock_Config();
extern "C" void MX_GPIO_Init();

int main() {
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    vTaskStartScheduler();
}
