#include "ramp.h"

Ramp::Ramp(const float max_vel, const float freq) :
		dt{1/freq},
		increment{max_vel/freq},
		target{0},
		current{0},
		timer{"ramp", this, &Ramp::update, static_cast<TickType_t>(1000.f/freq), true} {

	timer.start();
}

void Ramp::update() {
	if((target-current)>-increment && (target-current)<increment) {
		current = target;
		return;
	}

	if(target>current) {
		current +=increment;
	} else {
		current -=increment;
	}
}

void Ramp::setTarget(const float target) {
	this->target = target;
}

float Ramp::getOutput() const {
	return current;
}
