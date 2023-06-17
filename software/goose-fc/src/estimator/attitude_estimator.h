#pragma once

#include "TimerCPP.h"

#include "ekf.h"
#include "quaternion.h"
#include "vector.h"

class AttitudeEstimator {
    static constexpr float dt = 0.005;

    struct RotationModel : EKF<7>::SystemModel<3> {
        using EKF<7>::SystemModel<3>::SystemModel;

        Matrix<7, 1> f(Matrix<7, 1> x, Matrix<3, 1> u) const;
        Matrix<7, 7> f_tangent(Matrix<7, 1> x, Matrix<3, 1> u) const;
    };

    struct AccelerometerModel : EKF<7>::MeasurementModel<3> {
        using EKF<7>::MeasurementModel<3>::MeasurementModel;

        Matrix<3, 1> h(Matrix<7, 1> x) const;
        Matrix<3, 7> h_tangent(Matrix<7, 1> x) const;
    };

    struct MagnetometerModel : EKF<7>::MeasurementModel<3> {
        using EKF<7>::MeasurementModel<3>::MeasurementModel;

        Matrix<3, 1> h(Matrix<7, 1> x) const;
        Matrix<3, 7> h_tangent(Matrix<7, 1> x) const;
    };

    EKF<7> ekf;
    const RotationModel rotation_model;
    const AccelerometerModel accelerometer_model;
    const MagnetometerModel magnetometer_model;

    Vector gyration;
    Quaternion body_to_world;
	TimerMember<AttitudeEstimator> azimuth_setter;

    Vector removeMagneticDeclination(const Vector mag) const;
    Quaternion getAttitudeNED() const;
    void zeroAzimuth();

public:
    AttitudeEstimator();

    void feedAcceleration(Vector acc);
    void feedGyration(Vector gyr);
    void feedMagneticField(Vector mag);

    Quaternion getAttitude() const;
    Vector getRotationRates() const;
};
