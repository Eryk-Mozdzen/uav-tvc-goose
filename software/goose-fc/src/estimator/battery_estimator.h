#pragma once

#include "ekf.h"

class BatteryEstimator {
    static constexpr float freq = 50.f;
    static constexpr float dt = 1/freq;

    struct Battery {
        static constexpr float capacity = 1.f;
        static constexpr uint8_t cell_num = 5;
    };

    EKF<1, 1, 1, BatteryEstimator> ekf;

    Matrix<1, 1> f(const Matrix<1, 1> x, const Matrix<1, 1> u);
    Matrix<1, 1> h(const Matrix<1, 1> x);
    Matrix<1, 1> f_tangent(const Matrix<1, 1> x, const Matrix<1, 1> u);
    Matrix<1, 1> h_tangent(const Matrix<1, 1> x);

    static float poly_value(const float *poly, const uint8_t coeff_num, const float arg);

public:
    BatteryEstimator();

    void feedVoltage(const float voltage);
    void feedCurrent(const float current);

    float getStateOfCharge() const;
};
