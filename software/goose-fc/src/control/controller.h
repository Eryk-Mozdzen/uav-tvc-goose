#pragma once

#include "matrix.h"

class Controller {
    static constexpr float dt = 0.01f;
    static constexpr int X_NUM = 8;
    static constexpr int U_NUM = 5;
    static constexpr int G_NUM = 4;

    static constexpr Matrix<U_NUM, 1> operating_point = {
        0.0683,
        0.0683,
        0.0683,
        0.0683,
        0.6527,
    };

    static constexpr Matrix<G_NUM, X_NUM> G = {
        1, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0
    };

    static constexpr Matrix<U_NUM, X_NUM+G_NUM> K = {
        -0.0408,    0.1645,   -0.0812,   -0.0501,   -0.0000,   -0.0419,   -0.0000,    0.0000,   -0.0098,    0.0396,   -0.0289,   -0.0000,
        -0.1645,   -0.0407,   -0.0812,   -0.0000,   -0.0501,   -0.0419,   -0.0000,   -0.0000,   -0.0396,   -0.0098,   -0.0289,    0.0000,
         0.0408,   -0.1645,   -0.0812,    0.0501,    0.0000,   -0.0419,   -0.0000,   -0.0000,    0.0098,   -0.0396,   -0.0289,    0.0000,
         0.1645,    0.0407,   -0.0812,    0.0000,    0.0501,   -0.0419,   -0.0000,    0.0000,    0.0396,    0.0098,   -0.0289,   -0.0000,
        -0.0000,    0.0000,    0.0000,   -0.0000,    0.0000,   -0.0000,    0.1167,    0.1361,   -0.0000,    0.0000,    0.0000,    0.0316,
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
