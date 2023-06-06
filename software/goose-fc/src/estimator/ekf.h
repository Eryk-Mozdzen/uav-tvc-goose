#pragma once

#include "matrix.h"

template<int X, int U, int Z, class T>
class EKF {
		T *instance;
		Matrix<X, 1> (T::*f)(Matrix<X, 1>, Matrix<U, 1>);
		Matrix<Z, 1> (T::*h)(Matrix<X, 1>);
		Matrix<X, X> (T::*f_tangent)(Matrix<X, 1>, Matrix<U, 1>);
		Matrix<Z, X> (T::*h_tangent)(Matrix<X, 1>);

		const Matrix<X, X> Q;
		const Matrix<Z, Z> R;

		Matrix<X, X> P;
		Matrix<X, 1> x;

	public:

		EKF(
			T *instance_p,
			Matrix<X, 1> (T::*f_p)(Matrix<X, 1>, Matrix<U, 1>),
			Matrix<Z, 1> (T::*h_p)(Matrix<X, 1>),
			Matrix<X, X> (T::*f_tangent_p)(Matrix<X, 1>, Matrix<U, 1>),
			Matrix<Z, X> (T::*h_tangent_p)(Matrix<X, 1>),
			const Matrix<X, X> Q_p,
			const Matrix<Z, Z> R_p,
			const Matrix<X, 1> x_init
		) : instance{instance_p},
			f{f_p},
			h{h_p},
			f_tangent{f_tangent_p},
			h_tangent{h_tangent_p},
			Q{Q_p},
			R{R_p},
			x{x_init} {

		}

		void predict(const Matrix<U, 1> u) {
			const Matrix<X, X> F = (instance->*f_tangent)(x, u);

			x = (instance->*f)(x, u);
			P = F*P*F.transposition() + Q;
		}

		void update(const Matrix<Z, 1> z) {
			const Matrix<Z, X> H = (instance->*h_tangent)(x);

			const Matrix<Z, 1> y = z - (instance->*h)(x);
			const Matrix<Z, Z> S = H*P*H.transposition() + R;
			const Matrix<X, Z> K = P*H.transposition()*S.inverse();

			x = x + K*y;
			P = (Matrix<X, X>::identity() - K*H)*P;
		}

		const Matrix<X, 1> & getState() const {
			return x;
		}
};
