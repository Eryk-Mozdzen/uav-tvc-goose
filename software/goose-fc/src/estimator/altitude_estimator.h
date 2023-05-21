#pragma once

#include "extended_kalman_filter.h"
#include "vector.h"
#include "quaternion.h"

class AltitudeEstimator {

public:
    AltitudeEstimator();

    void feedAttitude(const Quaternion &attitude);
    void feedDistance(const float dist);
    void feedPressure(const float press);
    void feedAcceleration(const Vector &acc);

    float getAltitude() const;
};
