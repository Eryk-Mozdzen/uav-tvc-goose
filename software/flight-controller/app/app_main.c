#include "stm32u5xx_hal.h"
#include "main.h"

void app_main() {

    while(1) {
        HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
        HAL_Delay(500);
    }
}
