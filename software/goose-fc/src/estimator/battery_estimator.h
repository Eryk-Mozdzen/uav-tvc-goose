#pragma once

#include "ekf.h"
#include "affrls.h"

class BatteryEstimator {
    AFFRLS<5> affrls;
    EKF<3, 1, 1> ekf;

    static Matrix<3, 1> f(const Matrix<3, 1> x, const Matrix<1, 1> u);
    static Matrix<1, 1> h(const Matrix<3, 1> x);
    static Matrix<3, 3> f_tangent(const Matrix<3, 1> x, const Matrix<1, 1> u);
    static Matrix<1, 3> h_tangent(const Matrix<3, 1> x);

public:
    BatteryEstimator();

    void feedVoltage(const float voltage);
    void feedCurrent(const float current);

    float getStateOfCharge() const;
};
