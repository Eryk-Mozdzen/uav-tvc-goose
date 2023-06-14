#pragma once

#include "matrix.h"

template<int X>
class EKF {
	Matrix<X, X> P;
	Matrix<X, 1> x;

public:

	template<int U>
	struct SystemModel {
		const Matrix<X, X> Q;

		SystemModel(const Matrix<X, X> Q) : Q{Q} {}

		virtual Matrix<X, 1> f(Matrix<X, 1> x, Matrix<U, 1> u) const = 0;
		virtual Matrix<X, X> f_tangent(Matrix<X, 1> x, Matrix<U, 1> u) const = 0;
	};

	template<int Z>
	struct MeasurementModel {
		const Matrix<Z, Z> R;

		MeasurementModel(const Matrix<Z, Z> R) : R{R} {}

		virtual Matrix<Z, 1> h(Matrix<X, 1> x) const = 0;
		virtual Matrix<Z, X> h_tangent(Matrix<X, 1> x) const = 0;
	};

	EKF(const Matrix<X, 1> x_init) : P{Matrix<X, X>::identity()*1e+6}, x{x_init} {

	}

	template<int U>
	void predict(const SystemModel<U> &system, const Matrix<U, 1> u) {
		const Matrix<X, X> F = system.f_tangent(x, u);

		x = system.f(x, u);
		P = F*P*F.transposition() + system.Q;
	}

	template<int Z>
	void correct(const MeasurementModel<Z> &measurement, const Matrix<Z, 1> z) {
		const Matrix<Z, X> H = measurement.h_tangent(x);

		const Matrix<Z, 1> y = z - measurement.h(x);
		const Matrix<Z, Z> S = H*P*H.transposition() + measurement.R;
		const Matrix<X, Z> K = P*H.transposition()*S.inverse();

		x = x + K*y;
		P = (Matrix<X, X>::identity() - K*H)*P;
	}

	const Matrix<X, 1> & getState() const {
		return x;
	}
};
