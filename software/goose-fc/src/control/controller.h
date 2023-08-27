#pragma once

#include "matrix.h"

class Controller {
    static constexpr float dt = 0.01f;
    static constexpr int X_NUM = 8;
    static constexpr int U_NUM = 5;
    static constexpr int G_NUM = 4;

    static constexpr Matrix<U_NUM, 1> operating_point = {
   -0.0547,
   -0.0547,
   -0.0547,
   -0.0547,
    0.7833,
    };

    static constexpr Matrix<G_NUM, X_NUM> G = {
        1, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0
    };

    static constexpr Matrix<U_NUM, X_NUM+G_NUM> K = {
   -0.0234,    0.2058,   -0.0763,   -0.0522,   -0.0000,   -0.0431,    0.0000,    0.0000,   -0.0046,    0.0406,   -0.0289,    0.0000,
   -0.2058,   -0.0234,   -0.0763,   -0.0000,   -0.0521,   -0.0431,    0.0000,    0.0000,   -0.0406,   -0.0046,   -0.0289,    0.0000,
    0.0234,   -0.2058,   -0.0763,    0.0522,    0.0000,   -0.0431,    0.0000,    0.0000,    0.0046,   -0.0406,   -0.0289,    0.0000,
    0.2058,    0.0234,   -0.0763,   -0.0000,    0.0521,   -0.0431,    0.0000,    0.0000,    0.0406,    0.0046,   -0.0289,    0.0000,
   -0.0000,   -0.0000,    0.0000,    0.0000,   -0.0000,   -0.0000,    0.1201,    0.1489,   -0.0000,   -0.0000,    0.0000,    0.0316,
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
