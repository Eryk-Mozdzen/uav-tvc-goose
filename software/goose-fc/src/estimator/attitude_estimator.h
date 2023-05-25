#pragma once

#include "TimerCPP.h"

#include "ekf.h"
#include "quaternion.h"
#include "vector.h"

class AttitudeEstimator {
    static constexpr float dt = 0.005;

    EKF<7, 3, 6, AttitudeEstimator> ekf;

    Vector acceleration;
	Vector magnetic_field;
	bool acc_ready;
	bool mag_ready;

    Quaternion body_to_world;
	TimerMember<AttitudeEstimator> azimuth_setter;

    Matrix<7, 1> f(const Matrix<7, 1> state, const Matrix<3, 1> gyr);
    Matrix<6, 1> h(const Matrix<7, 1> state);
    Matrix<7, 7> f_tangent(const Matrix<7, 1> state, const Matrix<3, 1> gyr);
    Matrix<6, 7> h_tangent(const Matrix<7, 1> state);

    Vector removeMagneticDeclination(Vector mag) const;
    Quaternion getAttitudeNED() const;
    void updateEKF();
    void zeroAzimuth();

public:
    AttitudeEstimator();

    void feedAcceleration(const Vector &acc);
    void feedGyration(const Vector &gyr);
    void feedMagneticField(const Vector &mag);

    Quaternion getAttitude() const;
};
