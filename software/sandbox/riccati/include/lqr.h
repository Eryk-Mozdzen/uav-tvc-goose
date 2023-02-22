#pragma once

#include "matrix.h"

template<int X, int U>
constexpr Matrix<X, X> solveRiccati(const Matrix<X, X> &A, const Matrix<X, U> &B, const Matrix<X, X> &Q, const Matrix<U, U> &R) {
	constexpr float dt = 1;
	constexpr float tolerance = 1;
	constexpr int max_iter = 100;
	
	const Matrix<X, X> AT = A.transposition();
	const Matrix<X, X> S = B * R.inverse() * B.transposition();
	
	Matrix<X, X> P = Q;

    for(int i=0; i<max_iter; i++) {
		const Matrix<X, X> diff = AT*P + P*A - P*S*P + Q;

        if(diff.max_element()<tolerance)
            break;
		
		P +=diff*dt;
    }

    return P;
}

template<int X, int U>
constexpr Matrix<U, X> LQR(const Matrix<X, X> &A, const Matrix<X, U> &B, const Matrix<X, X> &Q, const Matrix<U, U> &R) {
    const Matrix<X, X> P = solveRiccati(A, B, Q, R);

	const Matrix<U, X> K = R.inverse() * B.transposition() * P;

    return K;
}
