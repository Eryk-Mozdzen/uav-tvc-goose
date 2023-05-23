#include "altitude_estimator.h"
#include <cmath>

AltitudeEstimator::AltitudeEstimator() :
        kf {
            A, B, H,
            Matrix<2, 2>::identity()*1.f,
            Matrix<1, 1>::identity()*0.01f,
            {0.f, 0.f}
        } {

}

void AltitudeEstimator::feedAttitude(const Quaternion &att) {
    attitude = att;
}

void AltitudeEstimator::feedDistance(const float dist) {
    const Matrix<3, 1> rpy = attitude.getRollPitchYaw();
    const float roll = rpy(0, 0);
    const float pitch = rpy(1, 0);

    const float dist_comp = std::abs(dist*cosf(roll)*cosf(pitch));

    kf.update({dist_comp});
}

void AltitudeEstimator::feedPressure(const float press) {
    const float press_0 = 100030.f;
    const float height = 44330.f*(1.f - powf(press/press_0, 0.1903f));

    (void)height;

    //kf.update({height});
}

void AltitudeEstimator::feedAcceleration(const Vector &acc) {
    acceleration = acc;

    const Vector lin = getLinearAcceleration();

    kf.predict({-lin.z});
}

Vector AltitudeEstimator::getLinearAcceleration() const {
    constexpr Vector gravity = -Vector::Z()*9.81f;

    const Matrix<3, 3> rot = attitude.getRotation();

	return rot*acceleration - gravity;
}

float AltitudeEstimator::getAltitude() const {
    const Matrix<2, 1> x = kf.getState();

    return x(0, 0);
}
