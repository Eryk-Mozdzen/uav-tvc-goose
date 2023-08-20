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
    0.7061
    };

    static constexpr Matrix<G_NUM, X_NUM> G = {
        1, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0
    };

    static constexpr Matrix<U_NUM, X_NUM+G_NUM> K = {
   -0.6629,    0.9178,   -0.2274,   -0.2636,   -0.0000,   -0.0687,   -0.0848,   -0.0660,   -0.1851,    0.2564,   -0.0654,   -0.0270,
   -0.9178,   -0.6628,   -0.2274,   -0.0000,   -0.2633,   -0.0687,   -0.0848,   -0.0660,   -0.2564,   -0.1851,   -0.0654,   -0.0270,
    0.6629,   -0.9178,   -0.2274,    0.2636,    0.0000,   -0.0687,   -0.0848,   -0.0660,    0.1851,   -0.2564,   -0.0654,   -0.0270,
    0.9178,    0.6628,   -0.2274,    0.0000,    0.2633,   -0.0687,   -0.0848,   -0.0660,    0.2564,    0.1851,   -0.0654,   -0.0270,
   -0.0000,   -0.0000,   -0.1323,   -0.0000,   -0.0000,   -0.0388,    0.2902,    0.2243,   -0.0000,    0.0000,   -0.0381,    0.0925,
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
