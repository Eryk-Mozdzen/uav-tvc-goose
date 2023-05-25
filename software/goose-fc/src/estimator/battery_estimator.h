#pragma once

#include "ekf.h"
#include "affrls.h"

class BatteryEstimator {
    static constexpr float freq = 50.f;
    static constexpr float T = 1/freq;

    AFFRLS<5> affrls;
    EKF<3, 1, 1, BatteryEstimator> ekf;

    bool voltage_ready, current_ready;
    float voltage_val, current_val;

    float V[3];
    float I[3];

    struct Parameters {
        float Rs;
        float R1xC1;
        float R2xC2;
    };

    Parameters parameters;

    Matrix<3, 1> f(const Matrix<3, 1> x, const Matrix<1, 1> u);
    Matrix<1, 1> h(const Matrix<3, 1> x);
    Matrix<3, 3> f_tangent(const Matrix<3, 1> x, const Matrix<1, 1> u);
    Matrix<1, 3> h_tangent(const Matrix<3, 1> x);

    void params_update();
    static Parameters discret2phisic(const Matrix<5, 1> &phi);

public:
    BatteryEstimator();

    void feedVoltage(const float voltage);
    void feedCurrent(const float current);

    float getStateOfCharge() const;
};
