#pragma once

#include "matrix.h"

template<int X, int U, int Z>
class KalmanFilter {
	const Matrix<X, X> F;	// state-transition model
	const Matrix<X, U> B;	// control-input model
	const Matrix<Z, X> H;	// observation model

	Matrix<X, X> P;			// a posteriori estimate covariance matrix
	Matrix<X, 1> x;			// a posteriori state estimate

public:

	KalmanFilter(
		const Matrix<X, X> F_p,
		const Matrix<X, U> B_p,
		const Matrix<Z, X> H_p,
		const Matrix<X, 1> x_init_p
	) : F{F_p}, B{B_p}, H{H_p}, x{x_init_p} {

	}

	void predict(const Matrix<U, 1> u, const Matrix<X, X> Q) {
		x = F*x + B*u;
		P = F*P*F.transposition() + Q;
	}

	void update(const Matrix<Z, 1> z, const Matrix<Z, Z> R) {
		const Matrix<Z, 1> y = z - H*x;
		const Matrix<Z, Z> S = H*P*H.transposition() + R;
		const Matrix<X, Z> K = P*H.transposition()*S.inverse();

		x = x + K*y;
		P = (Matrix<X, X>::identity() - K*H)*P;
	}

	const Matrix<X, 1> & getState() const {
		return x;
	}
};
