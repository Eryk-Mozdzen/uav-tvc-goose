#include "battery_estimator.h"
#include <functional>

BatteryEstimator::BatteryEstimator() :
        affrls{{1.f, 1.f, 1.f, 1.f, 1.f}, 1.f},
        ekf{
            &BatteryEstimator::f,
            &BatteryEstimator::h,
            &BatteryEstimator::f_tangent,
            &BatteryEstimator::h_tangent,
            Matrix<3, 3>::identity(),
            Matrix<1, 1>::identity(),
            {1.f, 0.f, 0.f}
        } {

}

Matrix<3, 1> BatteryEstimator::f(const Matrix<3, 1> x, const Matrix<1, 1> u) {
    (void)x;
    (void)u;

    return Matrix<3, 1>();
}

Matrix<1, 1> BatteryEstimator::h(const Matrix<3, 1> x) {
    (void)x;

    return Matrix<1, 1>();
}

Matrix<3, 3> BatteryEstimator::f_tangent(const Matrix<3, 1> x, const Matrix<1, 1> u) {
    (void)x;
    (void)u;

    return Matrix<3, 3>();
}

Matrix<1, 3> BatteryEstimator::h_tangent(const Matrix<3, 1> x) {
    (void)x;

    return Matrix<1, 3>();
}

void BatteryEstimator::feedVoltage(const float voltage) {
    (void)voltage;
}

void BatteryEstimator::feedCurrent(const float current) {
    (void)current;
}

float BatteryEstimator::getStateOfCharge() const {
    return 0.f;
}
