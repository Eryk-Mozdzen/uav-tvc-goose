#include "altitude_estimator.h"
#include <cmath>

AltitudeEstimator::AltitudeEstimator() :
        kf {A, B, H, {0.f, 0.f}},
        ground_pressure{100030.f},
        distance_compensated{0.f},
        source{Source::DISTANCE},
        timer{"altitude source", this, &AltitudeEstimator::timer_timeout, 1000, pdTRUE},
        counter{0} {

    timer.start();
}

void AltitudeEstimator::timer_timeout() {
    if(source==Source::DISTANCE) {
        Logger::getInstance().log(Logger::INFO, "alt: switching source to barometer...");
        source = Source::BAROMETER;
        return;
    }

    if(source==Source::BAROMETER) {
        if(counter>20) {
            Logger::getInstance().log(Logger::INFO, "alt: switching source to distance sensor...");
            source = Source::DISTANCE;
        }
        counter = 0;
        return;
    }
}

void AltitudeEstimator::feedAttitude(const Quaternion &att) {
    attitude = att;
}

void AltitudeEstimator::feedDistance(const float dist) {
    if(source==Source::DISTANCE) {
        timer.reset();

        const Matrix<3, 1> rpy = attitude.getRollPitchYaw();
        const float roll = rpy(0, 0);
        const float pitch = rpy(1, 0);

        distance_compensated = std::abs(dist*cosf(roll)*cosf(pitch));

        kf.update({distance_compensated}, {5.f});
        return;
    }

    if(source==Source::BAROMETER) {
        counter++;
        return;
    }
}

void AltitudeEstimator::feedPressure(const float press) {
    if(source==Source::DISTANCE) {
        const float press_0 = press/powf(1.f - distance_compensated/44330.f, 5.255f);
        constexpr float alpha = 0.1f;

        ground_pressure = (1.f - alpha)*ground_pressure + alpha*press_0;
        return;
    }

    if(source==Source::BAROMETER) {
        const float height = 44330.f*(1.f - powf(press/ground_pressure, 0.1903f));

        kf.update({height}, {1000.f});
        return;
    }
}

void AltitudeEstimator::feedAcceleration(const Vector &acc) {
    acceleration = acc;

    const Vector lin = getLinearAcceleration();

    kf.predict({-lin.z}, Matrix<2, 2>::identity()*1.f);
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

float AltitudeEstimator::getVerticalVelocity() const {
    const Matrix<2, 1> x = kf.getState();

    return x(1, 0);
}

AltitudeEstimator::Source AltitudeEstimator::getSource() const {
    return source;
}
