#pragma once

#include "matrix.h"
#include <functional>

template<int X_DIM, int U_DIM, int Z_DIM>
class ExtendedKalmanFilter {
		const std::function<Matrix<X_DIM, 1>(const Matrix<X_DIM, 1>, const Matrix<U_DIM, 1>)> f;
		const std::function<Matrix<Z_DIM, 1>(const Matrix<X_DIM, 1>)> h;

		const std::function<Matrix<X_DIM, X_DIM>(const Matrix<X_DIM, 1>, const Matrix<U_DIM, 1>)> d_dx_f;
		const std::function<Matrix<Z_DIM, X_DIM>(const Matrix<X_DIM, 1>)> d_dx_h;

		const Matrix<X_DIM, X_DIM> Q;
		const Matrix<Z_DIM, Z_DIM> R;

		Matrix<X_DIM, X_DIM> P;
		Matrix<X_DIM, 1> x;

	public:
		ExtendedKalmanFilter(
			const std::function<Matrix<X_DIM, 1>(const Matrix<X_DIM, 1>, const Matrix<U_DIM, 1>)> f_p, 
			const std::function<Matrix<Z_DIM, 1>(const Matrix<X_DIM, 1>)> h_p,
			const std::function<Matrix<X_DIM, X_DIM>(const Matrix<X_DIM, 1>, const Matrix<U_DIM, 1>)> d_dx_f_p,
			const std::function<Matrix<Z_DIM, X_DIM>(const Matrix<X_DIM, 1>)> d_dx_h_p,
			const Matrix<X_DIM, X_DIM> Q_p,
			const Matrix<Z_DIM, Z_DIM> R_p,
			Matrix<X_DIM, 1> x_init
		) : f{f_p}, h{h_p}, d_dx_f{d_dx_f_p}, d_dx_h{d_dx_h_p}, Q{Q_p}, R{R_p}, x{x_init} {

		}

		void predict(const Matrix<U_DIM, 1> u) {
			const Matrix<X_DIM, X_DIM> F = d_dx_f(x, u);

			x = f(x, u);
			P = F*P*F.transposition() + Q;
		}

		void update(const Matrix<Z_DIM, 1> z) {
			const Matrix<Z_DIM, X_DIM> H = d_dx_h(x);

			const Matrix<Z_DIM, 1> y = z - h(x);
			const Matrix<Z_DIM, Z_DIM> S = H*P*H.transposition() + R;
			const Matrix<X_DIM, Z_DIM> K = P*H.transposition()*S.inverse();

			x = x + K*y;
			P = (Matrix<X_DIM, X_DIM>::identity() - K*H)*P;
		}

		const Matrix<X_DIM, 1> & getState() const {
			return x;
		}
};
