#pragma once

#include "matrix.h"

class Controller {
    static constexpr float dt = 0.01f;
    static constexpr int X_NUM = 8;
    static constexpr int U_NUM = 5;
    static constexpr int G_NUM = 4;

    static constexpr Matrix<U_NUM, 1> operating_point = {
   -0.2094,
   -0.2094,
   -0.2094,
   -0.2094,
    0.7095,
    };

    static constexpr Matrix<G_NUM, X_NUM> G = {
        1, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0
    };

    static constexpr Matrix<U_NUM, X_NUM+G_NUM> K = {
   -0.0647,    0.2574,   -0.1311,   -0.0815,   -0.0000,   -0.0518,   -0.0385,   -0.0416,   -0.0172,    0.0686,   -0.0492,   -0.0128,
   -0.2574,   -0.0647,   -0.1311,    0.0000,   -0.0814,   -0.0518,   -0.0385,   -0.0416,   -0.0686,   -0.0172,   -0.0492,   -0.0128,
    0.0647,   -0.2574,   -0.1311,    0.0815,    0.0000,   -0.0518,   -0.0385,   -0.0416,    0.0172,   -0.0686,   -0.0492,   -0.0128,
    0.2574,    0.0647,   -0.1311,   -0.0000,    0.0814,   -0.0518,   -0.0385,   -0.0416,    0.0686,    0.0172,   -0.0492,   -0.0128,
   -0.0000,    0.0000,   -0.0153,    0.0000,   -0.0000,   -0.0059,    0.1314,    0.1413,   -0.0000,    0.0000,   -0.0057,    0.0440,
    };

    Matrix<G_NUM, 1> error_integral;
    Matrix<X_NUM, 1> setpoint;
    Matrix<X_NUM, 1> process_value;

    Controller();

public:
    Controller(Controller &) = delete;
	void operator=(const Controller &) = delete;

    void setSetpoint(const Matrix<X_NUM, 1> sp);
    void setProcessValue(const Matrix<X_NUM, 1> pv);
    void resetIntegral();
    Matrix<U_NUM, 1> calculate();

    Matrix<X_NUM, 1> getSetpoint() const;
    Matrix<X_NUM, 1> getProcessValue() const;

    static Controller& getInstance();
};
