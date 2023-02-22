#pragma once

#include "matrix.h"

template<int X_DIM, int U_DIM, int Z_DIM>
class KalmanFilter {
		const Matrix<X_DIM, X_DIM> F;	// state-transition model
		const Matrix<X_DIM, U_DIM> B;	// control-input model
		const Matrix<Z_DIM, X_DIM> H;	// observation model

		const Matrix<X_DIM, X_DIM> Q;	// covariance of the process noise
		const Matrix<Z_DIM, Z_DIM> R;	// covariance of the observation noise

		Matrix<X_DIM, X_DIM> P;			// a posteriori estimate covariance matrix
		Matrix<X_DIM, 1> x;				// a posteriori state estimate

	public:

		KalmanFilter(
			const Matrix<X_DIM, X_DIM> f, 
			const Matrix<X_DIM, U_DIM> b,
			const Matrix<Z_DIM, X_DIM> h,
			const Matrix<X_DIM, X_DIM> q,
			const Matrix<Z_DIM, Z_DIM> r,
			Matrix<X_DIM, 1> x_init
		) : F{f}, B{b}, H{h}, Q{q}, R{r}, x{x_init} {

		}

		void predict(const Matrix<U_DIM, 1> u) {
			x = F*x + B*u;
			P = F*P*F.transposition() + Q;
		}

		void update(const Matrix<Z_DIM, 1> z) {
			const Matrix<Z_DIM, 1> y = z - H*x;
			const Matrix<Z_DIM, Z_DIM> S = H*P*H.transposition() + R;
			const Matrix<X_DIM, Z_DIM> K = P*H.transposition()*S.inverse();

			x = x + K*y;
			P = (Matrix<X_DIM, X_DIM>::identity() - K*H)*P;
		}

		const Matrix<X_DIM, 1> & getState() const {
			return x;
		}

};
