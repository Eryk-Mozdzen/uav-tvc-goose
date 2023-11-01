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
		Lights::set(Lights::Blue, true);
		vTaskDelay(50);

		Lights::set(Lights::Blue, false);
		vTaskDelay(950);
	}
}
