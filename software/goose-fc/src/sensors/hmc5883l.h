#pragma once

#include "vector.h"

class HMC5883L {
		Vector magnetic_field;

	public:

		HMC5883L();

		void init();

		bool readData();
		Vector getMagneticField() const;
};

void magTaskFcn(void *param);
