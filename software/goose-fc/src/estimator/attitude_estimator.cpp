#include "attitude_estimator.h"

AttitudeEstimator::AttitudeEstimator() :
        ekf{{0, -1, 0, 0, 0, 0, 0}},
        rotation_model{Matrix<7, 7>::identity()*0.0001f},
        accelerometer_model{Matrix<3, 3>::identity()*1.f},
        magnetometer_model{Matrix<3, 3>::identity()*1.f},
        azimuth_setter{
            "attitude azimuth setter",
            this,
            &AttitudeEstimator::zeroAzimuth,
            3000,
            pdFALSE
        } {

    azimuth_setter.start();
}

Matrix<7, 1> AttitudeEstimator::RotationModel::f(Matrix<7, 1> x, Matrix<3, 1> u) const {
    const Quaternion q(x(0, 0), x(1, 0), x(2, 0), x(3, 0));

    const Matrix<7, 7> A = {
        1, 0, 0, 0,  0.5f*dt*q.i,  0.5f*dt*q.j,  0.5f*dt*q.k,
        0, 1, 0, 0, -0.5f*dt*q.w,  0.5f*dt*q.k, -0.5f*dt*q.j,
        0, 0, 1, 0, -0.5f*dt*q.k, -0.5f*dt*q.w,  0.5f*dt*q.i,
        0, 0, 0, 1,  0.5f*dt*q.j, -0.5f*dt*q.i, -0.5f*dt*q.w,

        0, 0, 0, 0,  1,			   0, 		     0,
        0, 0, 0, 0,  0,			   1,		     0,
        0, 0, 0, 0,  0,			   0,   		 1
    };

    const Matrix<7, 3> B = {
        -q.i, -q.j, -q.k,
         q.w, -q.k,  q.j,
         q.k,  q.w, -q.i,
        -q.j,  q.i,  q.w,
        0,    0,    0,
        0,    0,    0,
        0,    0,    0
    };

    return A*x + 0.5f*dt*B*u;
}

Matrix<7, 7> AttitudeEstimator::RotationModel::f_tangent(const Matrix<7, 1> x, const Matrix<3, 1> u) const {
    (void)u;

    const Quaternion q(x(0, 0), x(1, 0), x(2, 0), x(3, 0));

    const Matrix<7, 7> A = {
        1, 0, 0, 0,  0.5f*dt*q.i,  0.5f*dt*q.j,  0.5f*dt*q.k,
        0, 1, 0, 0, -0.5f*dt*q.w,  0.5f*dt*q.k, -0.5f*dt*q.j,
        0, 0, 1, 0, -0.5f*dt*q.k, -0.5f*dt*q.w,  0.5f*dt*q.i,
        0, 0, 0, 1,  0.5f*dt*q.j, -0.5f*dt*q.i, -0.5f*dt*q.w,

        0, 0, 0, 0,  1,			   0, 		     0,
        0, 0, 0, 0,  0,			   1,		     0,
        0, 0, 0, 0,  0,			   0,   		 1
    };

    return A;
}

Matrix<3, 1> AttitudeEstimator::AccelerometerModel::h(Matrix<7, 1> x) const {
    const Quaternion q(x(0, 0), x(1, 0), x(2, 0), x(3, 0));

    const Matrix<3, 7> C = {
         2.f*q.j, -2.f*q.k,  2.f*q.w, -2.f*q.i, 0, 0, 0,
        -2.f*q.i, -2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0,
        -2.f*q.w,  2.f*q.i,  2.f*q.j, -2.f*q.k, 0, 0, 0
    };

    return -1.f*C*x;
}

Matrix<3, 7> AttitudeEstimator::AccelerometerModel::h_tangent(Matrix<7, 1> x) const {
    const Quaternion q(x(0, 0), x(1, 0), x(2, 0), x(3, 0));

    const Matrix<3, 7> C = {
         2.f*q.j, -2.f*q.k,  2.f*q.w, -2.f*q.i, 0, 0, 0,
        -2.f*q.i, -2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0,
        -2.f*q.w,  2.f*q.i,  2.f*q.j, -2.f*q.k, 0, 0, 0
    };

    return -1.f*C;
}

Matrix<3, 1> AttitudeEstimator::MagnetometerModel::h(const Matrix<7, 1> x) const {
    const Quaternion q(x(0, 0), x(1, 0), x(2, 0), x(3, 0));

    const Matrix<3, 7> C = {
        -2.f*q.k, -2.f*q.j, -2.f*q.i, -2.f*q.w, 0, 0, 0,
        -2.f*q.w,  2.f*q.i, -2.f*q.j,  2.f*q.k, 0, 0, 0,
         2.f*q.i,  2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0
    };

    return C*x;
}

Matrix<3, 7> AttitudeEstimator::MagnetometerModel::h_tangent(const Matrix<7, 1> x) const {
    const Quaternion q(x(0, 0), x(1, 0), x(2, 0), x(3, 0));

    const Matrix<3, 7> C = {
        -2.f*q.k, -2.f*q.j, -2.f*q.i, -2.f*q.w, 0, 0, 0,
        -2.f*q.w,  2.f*q.i, -2.f*q.j,  2.f*q.k, 0, 0, 0,
         2.f*q.i,  2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0
    };

    return C;
}

Vector AttitudeEstimator::removeMagneticDeclination(const Vector b_mag) const {
	const Quaternion q = getAttitudeNED();
	const Matrix<3, 3> rot = q.getRotation();

	Vector w_mag = rot*b_mag;

	w_mag.z = 0.f;

	return rot.transposition()*w_mag;
}

void AttitudeEstimator::zeroAzimuth() {
	const Quaternion q = getAttitudeNED();

	const float azimuth = std::atan2(2.f*(q.w*q.k + q.i*q.j), q.w*q.w + q.i*q.i - q.j*q.j - q.k*q.k);

    body_to_world = Quaternion(-azimuth, Vector::Z());

    constexpr float pi = 3.14159265359f;

    Logger::getInstance().log(Logger::INFO, "est: set azimuth to %.0f deg", (double)(azimuth*180.f/pi));
}

Quaternion AttitudeEstimator::getAttitudeNED() const {
	const Matrix<7, 1> state = ekf.getState();
	const Quaternion q = Quaternion(state(0, 0), state(1, 0), state(2, 0), state(3, 0)).getNormalized();

	return q;
}

void AttitudeEstimator::feedAcceleration(Vector acc) {
    const float len = acc.getLength()/9.81f;

    if(len<1.03f && len>0.97f) {
        acc.normalize();
        ekf.correct(accelerometer_model, {acc.x, acc.y, acc.z});
    }
}

void AttitudeEstimator::feedGyration(Vector gyr) {
    gyration = gyr;

    ekf.predict(rotation_model, {gyr.x, gyr.y, gyr.z});
}

void AttitudeEstimator::feedMagneticField(Vector mag) {
    mag = removeMagneticDeclination(mag);
    mag.normalize();
    ekf.correct(magnetometer_model, {mag.x, mag.y, mag.z});
}

Quaternion AttitudeEstimator::getAttitude() const {
	const Matrix<7, 1> state = ekf.getState();
	const Quaternion q = Quaternion(state(0, 0), state(1, 0), state(2, 0), state(3, 0)).getNormalized();

    const Quaternion result = body_to_world^q;

    return result;
}

Vector AttitudeEstimator::getRotationRates() const {
    return gyration;
}
