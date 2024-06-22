#pragma once

#include "TrajectoryGenerator.h"

class Circle : public TrajectoryGenerator {
	static constexpr double pi = 3.14159265359;

	const double x;
	const double y;
	const double R;
	const double w;

	Eigen::VectorX<double> value(const double &t) const;

public:
	Circle(const double x, const double y, const double R, const double T);
};
