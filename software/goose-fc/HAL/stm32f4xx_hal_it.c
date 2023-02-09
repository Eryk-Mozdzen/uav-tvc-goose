#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim11;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

void TIM1_TRG_COM_TIM11_IRQHandler() {
	HAL_TIM_IRQHandler(&htim11);
}

void OTG_FS_IRQHandler() {
	HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}
