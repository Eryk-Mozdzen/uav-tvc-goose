#pragma once

#include <vector>

#include <Eigen/Dense>

#include "Interface.h"

class Accelerometer : public Interface {
    static constexpr double g = 9.80665;

    Eigen::Vector3d current;
    std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> samples;

    void leastSquares();

    Eigen::Matrix3d scale;
    Eigen::Vector3d offset;

public:
    Accelerometer(QWidget *parent = nullptr);

    Interface * create() const;
    void receive(const protocol_readings_t &readings);
    void update(protocol_calibration_t &calibration) const;
};
