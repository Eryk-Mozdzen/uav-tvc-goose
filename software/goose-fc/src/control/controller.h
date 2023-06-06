#pragma once

#include "matrix.h"

class Controller {
    static constexpr int X_NUM = 8;
    static constexpr int U_NUM = 5;

    static constexpr Matrix<U_NUM, 1> operating_point = {
        0.2527f,
        0.2527f,
        0.2527f,
        0.2527f,
        0.2229f
    };

    static constexpr Matrix<U_NUM, X_NUM> K = {
         1.4397f,    1.7109f,   -0.0000f,   -0.0000f,    1.2727f,   -0.4991f,    0.0305f,    0.0479f,
         1.7109f,   -1.4397f,   -0.0000f,    1.2727f,   -0.0000f,   -0.4991f,    0.0305f,    0.0479f,
        -1.4397f,   -1.7109f,   -0.0000f,    0.0000f,   -1.2727f,   -0.4991f,    0.0305f,    0.0479f,
        -1.7109f,    1.4397f,   -0.0000f,   -1.2727f,    0.0000f,   -0.4991f,    0.0305f,    0.0479f,
         0.0000f,   -0.0000f,    0.0000f,    0.0000f,   -0.0000f,    0.0061f,    0.0998f,    0.1106f
    };

    Matrix<X_NUM, 1> setpoint;
    Matrix<X_NUM, 1> process_value;

    Controller();

public:
    Controller(Controller &) = delete;
	void operator=(const Controller &) = delete;

    void setSetpoint(const Matrix<X_NUM, 1> sp);
    void setProcessValue(const Matrix<X_NUM, 1> pv);
    Matrix<U_NUM, 1> calculate() const;

    static Controller& getInstance();
};
