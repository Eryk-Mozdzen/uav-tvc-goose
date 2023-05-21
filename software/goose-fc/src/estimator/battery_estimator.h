#pragma once

#include "extended_kalman_filter.h"

class BatteryEstimator {

public:
    BatteryEstimator();

    void feedVoltage(const float voltage);
    void feedCurrent(const float current);

    float getStateOfCharge() const;
};
