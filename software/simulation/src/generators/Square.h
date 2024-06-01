#pragma once

#include "TrajectoryGenerator.h"

class Square : public TrajectoryGenerator {
	static constexpr double pi = 3.14159265359;

	const double a, R;
	const double v, w;
	const double T;

	Eigen::VectorX<double> value(const double &time) const;

public:
	Square(const double a, const double R, const double T);
};
