#pragma once

#include <Eigen/Dense>

struct Sample {
	float x, y, z;
};

struct Params {
	Eigen::Vector3d offset;
	Eigen::Vector3d scale;
	Eigen::Matrix3d rotation;

	Params() : offset{0, 0, 0}, scale{1, 1, 1}, rotation{Eigen::Matrix3d::Identity()} {

	}

	Eigen::MatrixXd getM() const {
		const Eigen::Matrix3d L {
			{1/scale(0), 0, 0},
			{0, 1/scale(1), 0},
			{0, 0, 1/scale(2)},
		};

		return rotation.transpose()*L*rotation;
	}

	Sample operator*(const Sample &sample) const {
		const Eigen::Vector3d input {
			sample.x,
			sample.y,
			sample.z
		};

		const Eigen::Vector3d output = getM()*(input - offset);

		return {static_cast<float>(output(0)), static_cast<float>(output(1)), static_cast<float>(output(2))};
	}
};
