#pragma once

#include "TaskCPP.h"
#include "vector.h"

class MPU6050 : public TaskClassS<512> {
		Vector gyration;	
		Vector acceleration;

	public:

		MPU6050();

		void init();

		bool readData();
		Vector getGyration() const;
		Vector getAcceleration() const;

		void task();
};
