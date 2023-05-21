#include "battery_estimator.h"

BatteryEstimator::BatteryEstimator() {

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
