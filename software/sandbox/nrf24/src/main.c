#include <FreeRTOS.h>
#include <stm32u0xx_hal.h>
#include <task.h>

void SystemClock_Config();
void MX_GPIO_Init();

int main() {
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    vTaskStartScheduler();

    return 0;
}
