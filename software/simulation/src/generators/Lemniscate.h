#pragma once

#include "TrajectoryGenerator.h"

class Lemniscate : public TrajectoryGenerator {
	static constexpr double sqrt2 = 1.41421356237;
	static constexpr double pi = 3.14159265359;
	static constexpr double deg2rad = pi/180;

	const double a;
	const double w;

	static double fix(double angle);

	Eigen::VectorX<double> value(const double &time) const;

public:
	Lemniscate(const double c, const double T);
};
