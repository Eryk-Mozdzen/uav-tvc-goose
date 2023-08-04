#pragma once

#include "matrix.h"

class Controller {
    static constexpr float dt = 0.01f;
    static constexpr int X_NUM = 8;
    static constexpr int U_NUM = 5;
    static constexpr int G_NUM = 4;

    static constexpr Matrix<U_NUM, 1> operating_point = {
        0.1317f,
        0.1317f,
        0.1317f,
        0.1317f,
        0.7061f
    };

    static constexpr Matrix<G_NUM, X_NUM> G = {
        1, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0
    };

    static constexpr Matrix<U_NUM, X_NUM+G_NUM> K = {
         0.3198f,  0.2589f, -0.0820f, -0.0000f,  0.1704f, -0.0481f, 0.0539f, 0.0418f,  0.1738f,  0.1407f, -0.0470f, 0.0171f,
         0.2590f, -0.3198f, -0.0820f,  0.1705f,  0.0000f, -0.0481f, 0.0539f, 0.0418f,  0.1407f, -0.1738f, -0.0470f, 0.0171f,
        -0.3198f, -0.2589f, -0.0820f,  0.0000f, -0.1704f, -0.0481f, 0.0539f, 0.0418f, -0.1738f, -0.1407f, -0.0470f, 0.0171f,
        -0.2590f,  0.3198f, -0.0820f, -0.1705f, -0.0000f, -0.0481f, 0.0539f, 0.0418f, -0.1407f,  0.1738f, -0.0470f, 0.0171f,
        -0.0000f, -0.0000f,  0.0595f,  0.0000f, -0.0000f,  0.0344f, 0.2943f, 0.2262f, -0.0000f, -0.0000f,  0.0343f, 0.0939f
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
    Matrix<U_NUM, 1> calculate();

    Matrix<X_NUM, 1> getSetpoint() const;
    Matrix<X_NUM, 1> getProcessValue() const;

    static Controller& getInstance();
};
