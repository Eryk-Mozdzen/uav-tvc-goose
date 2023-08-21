#pragma once

#include "TimerCPP.h"

class Ramp {
	const float dt;
	const float increment;
	float target;
	float current;

	TimerMember<Ramp> timer;

	void update();

public:

	Ramp(const float max_vel, const float freq);

	void setTarget(const float target);
	float getOutput() const;
};
