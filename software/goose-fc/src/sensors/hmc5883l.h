#pragma once

#include "TaskCPP.h"
#include "vector.h"

class HMC5883L : public TaskClassS<512> {
		Vector magnetic_field;

	public:

		HMC5883L();

		void init();

		bool readData();
		Vector getMagneticField() const;

		void task();
};
