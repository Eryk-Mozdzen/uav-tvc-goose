// Adaptive Forgetting Factor Regression Least Squares

#include "matrix.h"

template<int N>
class AFFRLS {
    Matrix<N, 1> theta;
    Matrix<N, N> P;
    float lambda;

public:
    AFFRLS(const Matrix<N, 1> theta_0, const float lambda_0);

    void feed(const float y, const Matrix<N, 1> phi);

    const Matrix<N, 1> & getParameters() const;
};

template<int N>
AFFRLS<N>::AFFRLS(const Matrix<N, 1> theta_0, const float lambda_0) :
        theta{theta_0},
        P{Matrix<N, N>::identity()},
        lambda{lambda_0} {

}

template<int N>
void AFFRLS<N>::feed(const float y, const Matrix<N, 1> phi) {
    const Matrix<1, N> phi_t = phi.transposition();

    const Matrix<N, 1> K = P*phi/(lambda + (phi_t*P*phi)(0, 0));
    theta = theta + K*(y - (phi_t*theta)(0, 0));
    P = (Matrix<N, N>::identity() - K*phi_t)*P/lambda;

    const float e = y - (phi_t*theta)(0, 0);
    lambda = 1.f - e*e/(1.f + (phi_t*P*phi)(0, 0));
}

template<int N>
const Matrix<N, 1> & AFFRLS<N>::getParameters() const {
    return theta;
}