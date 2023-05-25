#pragma once

#include "matrix.h"
#include <functional>

template<int X, int U, int Z>
class EKF {
		const std::function<Matrix<X, 1>(const Matrix<X, 1>, const Matrix<U, 1>)> f;
		const std::function<Matrix<Z, 1>(const Matrix<X, 1>)> h;

		const std::function<Matrix<X, X>(const Matrix<X, 1>, const Matrix<U, 1>)> f_tangent;
		const std::function<Matrix<Z, X>(const Matrix<X, 1>)> h_tangent;

		const Matrix<X, X> Q;
		const Matrix<Z, Z> R;

		Matrix<X, X> P;
		Matrix<X, 1> x;

	public:

		EKF(
			const std::function<Matrix<X, 1>(const Matrix<X, 1>, const Matrix<U, 1>)> f_p,
			const std::function<Matrix<Z, 1>(const Matrix<X, 1>)> h_p,
			const std::function<Matrix<X, X>(const Matrix<X, 1>, const Matrix<U, 1>)> f_tangent_p,
			const std::function<Matrix<Z, X>(const Matrix<X, 1>)> h_tangent_p,
			const Matrix<X, X> Q_p,
			const Matrix<Z, Z> R_p,
			const Matrix<X, 1> x_init
		) : f{f_p}, h{h_p}, f_tangent{f_tangent_p}, h_tangent{h_tangent_p}, Q{Q_p}, R{R_p}, x{x_init} {

		}

		void predict(const Matrix<U, 1> u) {
			const Matrix<X, X> F = f_tangent(x, u);

			x = f(x, u);
			P = F*P*F.transposition() + Q;
		}

		void update(const Matrix<Z, 1> z) {
			const Matrix<Z, X> H = h_tangent(x);

			const Matrix<Z, 1> y = z - h(x);
			const Matrix<Z, Z> S = H*P*H.transposition() + R;
			const Matrix<X, Z> K = P*H.transposition()*S.inverse();

			x = x + K*y;
			P = (Matrix<X, X>::identity() - K*H)*P;
		}

		const Matrix<X, 1> & getState() const {
			return x;
		}
};
