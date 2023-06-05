#pragma once

#include "TimerCPP.h"
#include "kalman_filter.h"
#include "vector.h"
#include "quaternion.h"
#include "comm.h"

class AltitudeEstimator {
public:
    using Source = comm::Estimator::AltitudeSource;

    AltitudeEstimator();

    void feedAttitude(const Quaternion &att);
    void feedDistance(const float dist);
    void feedPressure(const float press);
    void feedAcceleration(const Vector &acc);

    Vector getLinearAcceleration() const;
    float getAltitude() const;
    float getVerticalVelocity() const;
    Source getSource() const;

private:
    static constexpr float acc_freq = 200.f;
    static constexpr float dt = 1.f/acc_freq;

    static constexpr Matrix<2, 2> A = {
        1.f, dt,
        0.f, 1.f
    };
    static constexpr Matrix<2, 1> B = {
        0.5f*dt*dt,
        dt
    };
    static constexpr Matrix<1, 2> H = {
        1.f,
        0.f
    };

    KalmanFilter<2, 1, 1> kf;

    float ground_pressure;
    float distance_compensated;
    Vector acceleration;
    Quaternion attitude;

    Source source;
    TimerMember<AltitudeEstimator> timer;
    int counter;

    void timer_timeout();
};
