#include <FreeRTOS.h>
#include <stm32u0xx_hal.h>
#include <task.h>

void SystemClock_Config();
void MX_GPIO_Init();
void MX_USART2_UART_Init();
void MX_SPI1_Init();
void MX_SPI2_Init();

int main() {
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_SPI1_Init();
    MX_SPI2_Init();

    vTaskStartScheduler();

    return 0;
}
