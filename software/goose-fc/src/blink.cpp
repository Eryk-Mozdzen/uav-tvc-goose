#include "TaskCPP.h"

#include "lights.h"

class Blink : public TaskClassS<128> {
public:
	Blink();
	void task();
};

Blink blink;

Blink::Blink() : TaskClassS{"blink", TaskPrio_Idle} {

}

void Blink::task() {

	while(1) {
		Lights::toggle(Lights::Blue);
		vTaskDelay(500);
	}
}
