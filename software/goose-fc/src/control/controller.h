#pragma once

#include "matrix.h"

class Controller {
    static constexpr float dt = 0.01f;
    static constexpr int X_NUM = 7;
    static constexpr int U_NUM = 5;
    static constexpr int G_NUM = 4;

    static constexpr Matrix<U_NUM, 1> operating_point = {
    0.0586,
    0.0586,
    0.0586,
    0.0586,
    0.8443,
    };

    static constexpr Matrix<G_NUM, X_NUM> G = {
        1, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 1, 0
    };

    static constexpr Matrix<U_NUM, X_NUM+G_NUM> K = {
   -0.0458,    0.2162,   -0.0452,    0.0000,   -0.0503,    0.0000,    0.0000,   -0.0207,    0.0978,   -0.0316,    0.0000,
   -0.2162,   -0.0457,   -0.0000,   -0.0450,   -0.0503,   -0.0000,   -0.0000,   -0.0978,   -0.0207,   -0.0316,    0.0000,
    0.0458,   -0.2162,    0.0452,   -0.0000,   -0.0503,   -0.0000,   -0.0000,    0.0207,   -0.0978,   -0.0316,   -0.0000,
    0.2162,    0.0457,    0.0000,    0.0450,   -0.0503,    0.0000,    0.0000,    0.0978,    0.0207,   -0.0316,    0.0000,
    0.0000,    0.0000,   -0.0000,    0.0000,    0.0000,    0.1195,    0.1469,   -0.0000,    0.0000,    0.0000,    0.0316,
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
