#pragma once

#include "ekf.h"
#include "vector.h"
#include "quaternion.h"

class PositionEstimator {
    static constexpr float acc_freq = 200.f;
    static constexpr float dt = 1.f/acc_freq;

    struct MovementModel : EKF<7>::SystemModel<3> {
        using EKF<7>::SystemModel<3>::SystemModel;

        Matrix<7, 1> f(Matrix<7, 1> x, Matrix<3, 1> u) const;
        Matrix<7, 7> f_tangent(Matrix<7, 1> x, Matrix<3, 1> u) const;
    };

    struct LaserModel : EKF<7>::MeasurementModel<1> {
        using EKF<7>::MeasurementModel<1>::MeasurementModel;

        Matrix<1, 1> h(Matrix<7, 1> x) const;
        Matrix<1, 7> h_tangent(Matrix<7, 1> x) const;
    };

    struct BarometerModel : EKF<7>::MeasurementModel<1> {
        using EKF<7>::MeasurementModel<1>::MeasurementModel;

        Matrix<1, 1> h(Matrix<7, 1> x) const;
        Matrix<1, 7> h_tangent(Matrix<7, 1> x) const;
    };

    EKF<7> ekf;
    const MovementModel movement_model;
    const LaserModel laser_model;
    const BarometerModel barometer_model;

    Vector linear;
    Quaternion attitude;

public:
    PositionEstimator();

    void setAttitude(const Quaternion &att);

    void feedDistance(const float dist);
    void feedPressure(const float press);
    void feedAcceleration(const Vector &acc);

    Vector getPosition() const;
    Vector getVelocity() const;
    Vector getLinearAcceleration() const;
    float getGroundPressure() const;
};
