#include "battery_estimator.h"

BatteryEstimator::BatteryEstimator() :
        affrls{{1.f, 1.f, 1.f, 1.f, 1.f}, 1.f},
        ekf{
            this,
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

void BatteryEstimator::params_update() {
    V[2] = V[1];
    V[1] = V[0];
    V[0] = voltage_val;

    I[2] = I[1];
    I[1] = I[0];
    I[0] = current_val;

    const float y = V[0];
    const Matrix<5, 1> phi = {V[1], V[2], I[0], I[1], I[2]};

    affrls.feed(y, phi);

    parameters = discret2phisic(affrls.getParameters());
}

BatteryEstimator::Parameters BatteryEstimator::discret2phisic(const Matrix<5, 1> &phi) {
    const float a1 = phi(0, 0);
    const float a2 = phi(1, 0);
    const float b0 = phi(2, 0);
    const float b1 = phi(3, 0);
    const float b2 = phi(4, 0);

    Parameters parameters;

    parameters.Rs = -(b0 - b1 + b2)/(a1 - a2 + 1.f);

    const float r1c1xr2c2 = T*T*(a1 - a2 + 1.f)/(4.f*(a1 + a2 - 1.f));
    const float r1c1_r2c2 = -T*(a2 + 1.f)/(a1 + a2 - 1.f);
    const float sqrt_delta = std::sqrt(r1c1_r2c2*r1c1_r2c2 - 4.f*r1c1xr2c2);

    parameters.R1xC1 = 0.5f*(r1c1_r2c2 + sqrt_delta);
    parameters.R2xC2 = 0.5f*(r1c1_r2c2 - sqrt_delta);

    return parameters;
}

void BatteryEstimator::feedVoltage(const float voltage) {
    voltage_val = voltage;
    voltage_ready = true;

    if(current_ready) {
        params_update();
        voltage_ready = false;
        current_ready = false;
    }
}

void BatteryEstimator::feedCurrent(const float current) {
    current_val = current;
    current_ready = true;

    if(voltage_ready) {
        voltage_ready = false;
        current_ready = false;
        params_update();
    }
}

float BatteryEstimator::getStateOfCharge() const {
    return 0.f;
}
