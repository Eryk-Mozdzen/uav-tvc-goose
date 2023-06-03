#include "attitude_estimator.h"

AttitudeEstimator::AttitudeEstimator() :
        ekf{
            &AttitudeEstimator::f,
            &AttitudeEstimator::h,
            &AttitudeEstimator::f_tangent,
            &AttitudeEstimator::h_tangent,
            Matrix<7, 7>::identity()*0.0001f,
            Matrix<6, 6>::identity()*1.f,
            {0, -1, 0, 0, 0, 0, 0}
        },
        acc_ready{false},
        mag_ready{false},
        azimuth_setter{
            "attitude azimuth setter",
            this,
            &AttitudeEstimator::zeroAzimuth,
            3000,
            pdFALSE
        } {

    azimuth_setter.start();
}

Matrix<7, 1> AttitudeEstimator::f(const Matrix<7, 1> state, const Matrix<3, 1> gyr) {
    (void)gyr;

    const Quaternion q(state(0, 0), state(1, 0), state(2, 0), state(3, 0));

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

    return A*state + 0.5f*dt*B*gyr;
}

Matrix<6, 1> AttitudeEstimator::h(const Matrix<7, 1> state) {
    const Quaternion q(state(0, 0), state(1, 0), state(2, 0), state(3, 0));

    const Matrix<6, 7> C = {
         2.f*q.j, -2.f*q.k,  2.f*q.w, -2.f*q.i, 0, 0, 0,
        -2.f*q.i, -2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0,
        -2.f*q.w,  2.f*q.i,  2.f*q.j, -2.f*q.k, 0, 0, 0,

        -2.f*q.k, -2.f*q.j, -2.f*q.i, -2.f*q.w, 0, 0, 0,
        -2.f*q.w,  2.f*q.i, -2.f*q.j,  2.f*q.k, 0, 0, 0,
         2.f*q.i,  2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0
    };

    return C*state;
}

Matrix<7, 7> AttitudeEstimator::f_tangent(const Matrix<7, 1> state, const Matrix<3, 1> gyr) {
    (void)gyr;

    const Quaternion q(state(0, 0), state(1, 0), state(2, 0), state(3, 0));

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

Matrix<6, 7> AttitudeEstimator::h_tangent(const Matrix<7, 1> state) {
    const Quaternion q(state(0, 0), state(1, 0), state(2, 0), state(3, 0));

    const Matrix<6, 7> C = {
         2.f*q.j, -2.f*q.k,  2.f*q.w, -2.f*q.i, 0, 0, 0,
        -2.f*q.i, -2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0,
        -2.f*q.w,  2.f*q.i,  2.f*q.j, -2.f*q.k, 0, 0, 0,

        -2.f*q.k, -2.f*q.j, -2.f*q.i, -2.f*q.w, 0, 0, 0,
        -2.f*q.w,  2.f*q.i, -2.f*q.j,  2.f*q.k, 0, 0, 0,
         2.f*q.i,  2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0
    };

    return C;
}

Vector AttitudeEstimator::removeMagneticDeclination(Vector b_mag) const {
	const Quaternion q = getAttitudeNED();
	const Matrix<3, 3> rot = q.getRotation();

	Vector w_mag = rot*b_mag;

	w_mag.z = 0.f;
	w_mag.normalize();

	b_mag = rot.transposition()*w_mag;

	return b_mag;
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

void AttitudeEstimator::updateEKF() {
    if(acc_ready && mag_ready) {
        acc_ready = false;
        mag_ready = false;

        acceleration.normalize();
        magnetic_field.normalize();

        const Vector mag = removeMagneticDeclination(magnetic_field);

        ekf.update({acceleration.x, acceleration.y, acceleration.z, mag.x, mag.y, mag.z});
    }
}

void AttitudeEstimator::feedAcceleration(const Vector &acc) {
    acceleration = acc;

    const float len = acceleration.getLength()/9.81f;

    if(len<1.03f && len>0.97f) {
        acc_ready = true;
    }

    updateEKF();
}

void AttitudeEstimator::feedGyration(const Vector &gyr) {
    ekf.predict(gyr);

    gyration = gyr;
}

void AttitudeEstimator::feedMagneticField(const Vector &mag) {
    magnetic_field = mag;
    mag_ready = true;

    updateEKF();
}

Quaternion AttitudeEstimator::getAttitude() const {
	const Matrix<7, 1> state = ekf.getState();
	const Quaternion q = Quaternion(state(0, 0), state(1, 0), state(2, 0), state(3, 0)).getNormalized();

    const Quaternion result = body_to_world^q;

    return result;
}

Vector AttitudeEstimator::getRotationRates() const {
    return Vector(-gyration.y, gyration.x, -gyration.z);
}

