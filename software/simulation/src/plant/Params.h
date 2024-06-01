#pragma once

struct Params {
    static constexpr double g = 9.8065;

    static constexpr double m = 0.332;
    static constexpr double l = 0.0500;
    static constexpr double r = 0.0665;

    static constexpr double J_xx = 0.0009887;
    static constexpr double J_yy = 0.0009817;
    static constexpr double J_zz = 0.0002177;
    static constexpr double J_r = 0.00001366;

    static constexpr double a_s = -0.08727;
    static constexpr double K_l = 0.4203;
    static constexpr double K_m = 0.000000041;
    static constexpr double K_w = 0.000003133;
};
