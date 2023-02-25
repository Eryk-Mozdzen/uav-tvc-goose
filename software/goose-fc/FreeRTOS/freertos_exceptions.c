#include "FreeRTOS.h"
#include "task.h"

void vApplicationMallocFailedHook() {

	while(1);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
	(void)xTask;
	(void)pcTaskName;

	while(1);
}
