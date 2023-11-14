#include "position_estimator.h"
#include <cmath>

PositionEstimator::PositionEstimator() :
        ekf{{0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 101325.f}},
        movement_model{Matrix<7, 7>::identity()*0.01f},
        distance_model{{0.1f}},
        barometer_model{{100.f}} {

}

Matrix<7, 1> PositionEstimator::MovementModel::f(Matrix<7, 1> x, Matrix<3, 1> u) const {

    constexpr Matrix<7, 7> A = {
        1, 0, 0, dt,  0,  0, 0,
        0, 1, 0,  0, dt,  0, 0,
        0, 0, 1,  0,  0, dt, 0,
        0, 0, 0,  1,  0,  0, 0,
        0, 0, 0,  0,  1,  0, 0,
        0, 0, 0,  0,  0,  1, 0,
        0, 0, 0,  0,  0,  0, 1
    };

    constexpr Matrix<7, 3> B = {
        0.5f*dt*dt, 0,          0,
        0,          0.5f*dt*dt, 0,
        0,          0,          0.5f*dt*dt,
        dt,         0,          0,
        0,          dt,         0,
        0,          0,          dt,
        0,          0,          0
    };

    return A*x + B*u;
}

Matrix<7, 7> PositionEstimator::MovementModel::f_tangent(Matrix<7, 1> x, Matrix<3, 1> u) const {
    (void)x;
    (void)u;

    constexpr Matrix<7, 7> A = {
        1, 0, 0, dt,  0,  0, 0,
        0, 1, 0,  0, dt,  0, 0,
        0, 0, 1,  0,  0, dt, 0,
        0, 0, 0,  1,  0,  0, 0,
        0, 0, 0,  0,  1,  0, 0,
        0, 0, 0,  0,  0,  1, 0,
        0, 0, 0,  0,  0,  0, 1
    };

    return A;
}

Matrix<1, 1> PositionEstimator::DistanceModel::h(Matrix<7, 1> x) const {

    constexpr Matrix<1, 7> C = {
        0, 0, 1, 0, 0, 0, 0
    };

    return C*x;
}

Matrix<1, 7> PositionEstimator::DistanceModel::h_tangent(Matrix<7, 1> x) const {
    (void)x;

    constexpr Matrix<1, 7> C = {
        0, 0, 1, 0, 0, 0, 0
    };

    return C;
}

Matrix<1, 1> PositionEstimator::BarometerModel::h(Matrix<7, 1> x) const {
    const float height = x(2, 0);
    const float press_0 = x(6, 0);

    const float press = press_0*powf(1.f - height/44330.f, 5.255f);

    return {press};
}

Matrix<1, 7> PositionEstimator::BarometerModel::h_tangent(Matrix<7, 1> x) const {
    const float height = x(2, 0);
    const float press_0 = x(6, 0);

    const float height_deriv = -(5.255f/44330.f)*press_0*powf(1.f - height/44330.f, 4.255f);
    const float press_0_deriv = powf(1.f - height/44330.f, 5.255f);

    const Matrix<1, 7> C = {
        0, 0, height_deriv, 0, 0, 0, press_0_deriv
    };

    return C;
}

void PositionEstimator::setAttitude(const Quaternion &att) {
    attitude = att;
}

void PositionEstimator::feedDistance(const float dist) {
    const Matrix<3, 1> rpy = attitude.getRollPitchYaw();
    const float roll = rpy(0, 0);
    const float pitch = rpy(1, 0);

    const float alt = dist*cosf(roll)*cosf(pitch);

    ekf.correct(distance_model, {alt});
}

void PositionEstimator::feedPressure(const float press) {
    ekf.correct(barometer_model, {press});
}

void PositionEstimator::feedAcceleration(const Vector &acc) {
    constexpr Vector gravity = Vector::Z()*9.80665f;
    const Matrix<3, 3> rot = attitude.getRotation();

	linear = rot*acc - gravity;

    ekf.predict(movement_model, {linear.x, linear.y, linear.z});
}

Vector PositionEstimator::getPosition() const {
    const Matrix<7, 1> x = ekf.getState();

    return Vector(x(0, 0), x(1, 0), x(2, 0));
}

Vector PositionEstimator::getVelocity() const {
    const Matrix<7, 1> x = ekf.getState();

    return Vector(x(3, 0), x(4, 0), x(5, 0));
}

Vector PositionEstimator::getLinearAcceleration() const {
    return linear;
}

float PositionEstimator::getGroundPressure() const {
    const Matrix<7, 1> x = ekf.getState();

    return x(6, 0);
}
