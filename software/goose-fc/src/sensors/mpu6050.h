#pragma once

#include "vector.h"

class MPU6050 {
		Vector gyration;	
		Vector acceleration;

	public:

		MPU6050();

		void init();

		bool readData();
		Vector getGyration() const;
		Vector getAcceleration() const;
};

void imuTaskFcn(void *param);
