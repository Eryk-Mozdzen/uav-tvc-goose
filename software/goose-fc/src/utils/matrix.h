#pragma once

#include <cmath>
#include "transmitter.h"

template<int N, int M>
class Matrix {
	float elements[N*M];

public:

	constexpr Matrix() : elements{0} {

	}

	template<typename... U>
    constexpr Matrix(U... list) : elements{static_cast<float>(list)...} {
        static_assert(sizeof...(list)==N*M, "wrong size");
    }
	
	constexpr float& operator()(const int row, const int col) {
		return elements[row*M + col];
	}

	constexpr const float& operator()(const int row, const int col) const {
		return elements[row*M + col];
	}

	constexpr Matrix<N, M> operator+(const Matrix<N, M> &second) const {
		Matrix<N, M> result;
		for(int i=0; i<N*M; i++)
			result.elements[i] = elements[i] + second.elements[i];
		return result;
	}

	constexpr void operator+=(const Matrix<N, M> &second) {
		for(int i=0; i<N*M; i++)
			elements[i] +=second.elements[i];
	}

	constexpr Matrix<N, M> operator-(const Matrix<N, M> &second) const {
		Matrix<N, M> result;
		for(int i=0; i<N*M; i++)
			result.elements[i] = elements[i] - second.elements[i];
		return result;
	}

	template<int K>
	constexpr Matrix<N, K> operator*(const Matrix<M, K> &second) const {
		Matrix<N, K> result;

		for(int i=0; i<N; i++)
			for(int j=0; j<K; j++)
				for(int k=0; k<M; k++)
					result(i, j) +=(*this)(i, k)*second(k, j);

		return result;
	}

	constexpr Matrix<N, M> operator*(const float &number) const {
		Matrix<N, M> result = *this;
		for(int i=0; i<N*M; i++)
			result.elements[i] = elements[i]*number;
		return result;
	}

	constexpr Matrix<M, N> transposition() const {
		Matrix<M, N> result;

		for(int i=0; i<N; i++)
			for(int j=0; j<M; j++)
				result(j, i) = (*this)(i, j);

		return result;
	}

	constexpr float max_element() const {
		float max = 0;

		for(int i=0; i<N*M; i++) {
			if(std::abs(elements[i])>max) {
				max = std::abs(elements[i]);
			}
		}

		return max;
	}

	constexpr void swapRows(const int row1, const int row2) {
		for(int i=0; i<M; i++) {
			const float tmp = (*this)(row1, i);
			(*this)(row1, i) = (*this)(row2, i);
			(*this)(row2, i) = tmp;
		}
	}

	constexpr static Matrix<N, N> identity() {
		Matrix<N, N> result;

		for(int i=0; i<N; i++)
			result(i, i) = 1;

		return result;
	}

	constexpr Matrix<N, N> inverse() const {

		Matrix<N, 2*N> combined;

		for(int i=0; i<N; i++) {
			for(int j=0; j<N; j++)
				combined(i, j) = (*this)(i, j);
			combined(i, i+N) = 1;
		}

		for(int i=0; i<N; i++) {
			float maxVal = 0.001f;
			int maxRow = i;

			for(int j=i; j<N; j++) {
				if(std::abs(combined(j, i))>maxVal) {
					maxVal = std::abs(combined(j, i));
					maxRow = j;
				}
			}

			if(maxVal<0.002f) {
				Transmitter::log(Transmitter::WARNING, "mat: can't inverse %dx%d matrix\n\r", N, N);
				return Matrix<N, N>();
			}

			combined.swapRows(maxRow, i);

			const float joint = combined(i, i);

			for(int k=0; k<2*N; k++)
				combined(i, k) /=joint;

			for(int j=i+1; j<N; j++) {
				const float multi = combined(j, i);

				for(int k=0; k<2*N; k++) {
					combined(j, k) -=multi*combined(i, k);
				}
			}
		}

		for(int i=N-1; i>=0; i--) {

			for(int j=i-1; j>=0; j--) {
				const float multi = combined(j, i);

				for(int k=0; k<2*N; k++) {
					combined(j, k) -=multi*combined(i, k);
				}
			}
		}

		Matrix<N, N> inverse;

		for(int i=0; i<N; i++)
			for(int j=0; j<N; j++)
				inverse(i, j) = combined(i, j+N);

		return inverse;
	}

	template<int K, int L>
	constexpr Matrix<K, L> slice(const int row, const int col) const {
		Matrix<K, L> result;

		for(int i=0; i<K; i++)
			for(int j=0; j<L; j++)
				result(i, j) = (*this)(row + i, col + j);

		return result;
	}
};

template<int N, int M>
constexpr Matrix<N, M> operator*(const float &number, const Matrix<N, M> &matrix) {
	Matrix<N, M> result;
	for(int i=0; i<N; i++)
		for(int j=0; j<M; j++)
			result(i, j) = matrix(i, j)*number;
	return result;
}
