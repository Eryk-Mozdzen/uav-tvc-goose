#include "battery_estimator.h"

BatteryEstimator::BatteryEstimator() :
        affrls{{1.f, 1.f, 1.f, 1.f, 1.f}, 1.f},
        ekf{
            this,
            &BatteryEstimator::f,
            &BatteryEstimator::h,
            &BatteryEstimator::f_tangent,
            &BatteryEstimator::h_tangent,
            Matrix<3, 3>::identity(),
            Matrix<1, 1>::identity(),
            {1.f, 0.f, 0.f}
        } {

}

Matrix<3, 1> BatteryEstimator::f(const Matrix<3, 1> x, const Matrix<1, 1> u) {

    const Matrix<3, 3> A = {
        1.f,    0.f,                                0.f,
        0.f,    std::exp(-T/(params.R1*params.C1)), 0.f,
        0.f,    0.f,                                std::exp(-T/(params.R2*params.C2))
    };

    const Matrix<3, 1> B = {
        -1.f/C,
        1.f/params.C1,
        1.f/params.C2
    };

    return A*x + B*u;
}

Matrix<1, 1> BatteryEstimator::h(const Matrix<3, 1> x) {

    const Matrix<1, 3> C = {
        0.f, -1.f, -1.f
    };

    return C*x;
}

Matrix<3, 3> BatteryEstimator::f_tangent(const Matrix<3, 1> x, const Matrix<1, 1> u) {
    (void)x;
    (void)u;

    const Matrix<3, 3> A = {
        1.f,    0.f,                                0.f,
        0.f,    std::exp(-T/(params.R1*params.C1)), 0.f,
        0.f,    0.f,                                std::exp(-T/(params.R2*params.C2))
    };

    return A;
}

Matrix<1, 3> BatteryEstimator::h_tangent(const Matrix<3, 1> x) {
    (void)x;

    const Matrix<1, 3> C = {
        0.f, -1.f, -1.f
    };

    return C;
}

void BatteryEstimator::params_update() {
    V[2] = V[1];
    V[1] = V[0];
    V[0] = voltage_val;

    I[2] = I[1];
    I[1] = I[0];
    I[0] = current_val;

    const float y = V[0];
    const Matrix<5, 1> phi = {V[1], V[2], I[0], I[1], I[2]};

    affrls.feed(y, phi);

    params = discret2phisic(affrls.getParameters());
}

BatteryEstimator::Parameters BatteryEstimator::discret2phisic(const Matrix<5, 1> &phi) {
    const float a1 = phi(0, 0);
    const float a2 = phi(1, 0);
    const float b0 = phi(2, 0);
    const float b1 = phi(3, 0);
    const float b2 = phi(4, 0);

    Parameters params;

    params.Rs = -(b0 - b1 + b2)/(a1 - a2 + 1.f);

    const float r1c1xr2c2 = T*T*(a1 - a2 + 1.f)/(4.f*(a1 + a2 - 1.f));
    const float r1c1_r2c2 = -T*(a2 + 1.f)/(a1 + a2 - 1.f);
    const float sqrt_delta = std::sqrt(r1c1_r2c2*r1c1_r2c2 - 4.f*r1c1xr2c2);

    const float R1xC1 = 0.5f*(r1c1_r2c2 + sqrt_delta);
    const float R2xC2 = 0.5f*(r1c1_r2c2 - sqrt_delta);

    {
        const float c3 = (b0 + b1 + b2)/(a1 + a2 - 1.f);
        const float c4 = -T*(b0 - b2)/(a1 + a2 - 1.f);

        const Matrix<2, 2> A = {
            1.f,    1.f,
            R2xC2,  R1xC1
        };

        const Matrix<2, 1> b = {
            c3 - params.Rs,
            c4 - params.Rs*(R1xC1 + R2xC2)
        };

        const Matrix<2, 1> r1r2 = A.inverse()*b;

        params.R1 = r1r2(0, 0);
        params.R2 = r1r2(1, 0);

        params.C1 = R1xC1/params.R1;
        params.C2 = R2xC2/params.R2;
    }

    return params;
}

void BatteryEstimator::feedVoltage(const float voltage) {
    voltage_val = voltage;
    voltage_ready = true;

    if(current_ready) {
        params_update();
        voltage_ready = false;
        current_ready = false;
    }
}

void BatteryEstimator::feedCurrent(const float current) {
    current_val = current;
    current_ready = true;

    if(voltage_ready) {
        voltage_ready = false;
        current_ready = false;
        params_update();
    }
}

float BatteryEstimator::getStateOfCharge() const {
    return 0.f;
}
