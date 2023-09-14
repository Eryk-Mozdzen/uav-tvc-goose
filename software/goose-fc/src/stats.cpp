#include "TaskCPP.h"
#include "logger.h"
#include <cstdlib>

class Stats : public TaskClassS<512> {
	static int compare(const void *a, const void *b);

public:
	Stats();

	void task();
};

//Stats stats;

Stats::Stats() : TaskClassS{"stats", TaskPrio_Idle} {

}

int Stats::compare(const void *a, const void *b) {
    const TaskStatus_t *task_a = (const TaskStatus_t *)a;
    const TaskStatus_t *task_b = (const TaskStatus_t *)b;

    return ((int64_t)task_b->ulRunTimeCounter) - ((int64_t)task_a->ulRunTimeCounter);
}

void Stats::task() {

	while(1) {
		uint32_t uxArraySize = 20;
		TaskStatus_t pxTaskStatusArray[20];

		uint32_t ulTotalTime;
		uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalTime);

		ulTotalTime /=100;

		qsort(pxTaskStatusArray, uxArraySize, sizeof(TaskStatus_t), compare);

		if(ulTotalTime>0) {
			for(uint32_t x=0; x<uxArraySize; x++) {
				Logger::getInstance().log(Logger::DEBUG, "%-25s%8lu %3lu%%%6u",
					pxTaskStatusArray[x].pcTaskName,
					pxTaskStatusArray[x].ulRunTimeCounter,
					pxTaskStatusArray[x].ulRunTimeCounter/ulTotalTime,
					pxTaskStatusArray[x].usStackHighWaterMark
				);
			}
		}

		vTaskDelay(10000);
	}
}
