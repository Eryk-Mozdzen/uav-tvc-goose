#include "battery_estimator.h"

BatteryEstimator::BatteryEstimator() :
        ekf{{0.f}},
        discharge_model{{0.0001f}},
        voltage_model{{1.f}} {

}

Matrix<1, 1> BatteryEstimator::DischargeModel::f(Matrix<1, 1> x, Matrix<1, 1> u) const {
    const Matrix<1, 1> A = {
        1.f
    };

    const Matrix<1, 1> B = {
        -dt/Battery::capacity,
    };

    return A*x + B*u;
}

Matrix<1, 1> BatteryEstimator::DischargeModel::f_tangent(Matrix<1, 1> x, Matrix<1, 1> u) const {
    (void)x;
    (void)u;

    return {-1.f};
}

Matrix<1, 1> BatteryEstimator::VoltageModel::h(Matrix<1, 1> x) const {
    const float SOC = x(0, 0);

    constexpr float poly[] = {
        -2134.64,
        +13392.91,
        -35843.97,
        +53329.10,
        -47905.40,
        +26043.50,
        -7713.30,
        +565.02,
        +379.80,
        -128.29,
        +16.67,
        +2.69
    };

    return {poly_value(poly, sizeof(poly)/sizeof(float), SOC)*Battery::cell_num};
}

Matrix<1, 1> BatteryEstimator::VoltageModel::h_tangent(Matrix<1, 1> x) const {
    const float SOC = x(0, 0);

    constexpr float poly[] = {
        -23481.07,
        +133929.13,
        -322595.69,
        +426632.82,
        -335337.79,
        +156260.98,
        -38566.51,
        +2260.09,
        +1139.41,
        -256.58,
        +16.67
    };

    return {poly_value(poly, sizeof(poly)/sizeof(float), SOC)*Battery::cell_num};
}

float BatteryEstimator::poly_value(const float *poly, const uint8_t coeff_num, const float arg) {
    float val = poly[coeff_num-1];
    float pow = arg;

    for(int i=coeff_num-2; i>=0; i--) {
        val +=poly[i]*pow;
        pow *=arg;
    }

    return val;
}

void BatteryEstimator::feedVoltage(const float voltage) {
    ekf.correct(voltage_model, {voltage});
}

void BatteryEstimator::feedCurrent(const float current) {
    ekf.predict(discharge_model, {current});
}

float BatteryEstimator::getStateOfCharge() const {
    const Matrix<1, 1> state = ekf.getState();
    const float SOC = state(0, 0);

    return SOC;
}
