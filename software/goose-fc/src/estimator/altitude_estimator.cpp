#include "altitude_estimator.h"

AltitudeEstimator::AltitudeEstimator() {

}

void AltitudeEstimator::feedAttitude(const Quaternion &attitude) {
    (void)attitude;
}

void AltitudeEstimator::feedDistance(const float dist) {
    (void)dist;
}

void AltitudeEstimator::feedPressure(const float press) {
    (void)press;
}

void AltitudeEstimator::feedAcceleration(const Vector &acc) {
    (void)acc;
}

float AltitudeEstimator::getAltitude() const {
    return 0.f;
}
