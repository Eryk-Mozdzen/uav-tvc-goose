#pragma once

#include "ekf.h"

class BatteryEstimator {
    static constexpr float freq = 50.f;
    static constexpr float dt = 1/freq;

    struct Battery {
        static constexpr float capacity = 0.85f;
        static constexpr uint8_t cell_num = 3;
    };

    class DischargeModel : public EKF<1>::SystemModel<1> {
        using EKF<1>::SystemModel<1>::SystemModel;

        Matrix<1, 1> f(Matrix<1, 1> x, Matrix<1, 1> u) const;
        Matrix<1, 1> f_tangent(Matrix<1, 1> x, Matrix<1, 1> u) const;
    };

    class VoltageModel : public EKF<1>::MeasurementModel<1> {
        using EKF<1>::MeasurementModel<1>::MeasurementModel;

        Matrix<1, 1> h(Matrix<1, 1> x) const;
        Matrix<1, 1> h_tangent(Matrix<1, 1> x) const;
    };

    EKF<1> ekf;
    const DischargeModel discharge_model;
    const VoltageModel voltage_model;

    static float poly_value(const float *poly, const uint8_t coeff_num, const float arg);

public:
    BatteryEstimator();

    void feedVoltage(const float voltage);
    void feedCurrent(const float current);

    float getStateOfCharge() const;
};
