#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <initializer_list>
#include <algorithm>

template<int N, int M>
class Matrix {
	std::array<float, N*M> elements;

public:

	constexpr Matrix() = default;

	constexpr Matrix(std::initializer_list<float> list) : elements{} {
		int i = 0;
		for(const float& elem : list) {
			elements[i++] = elem;
		}
	}
	
	constexpr float& operator()(const int row, const int col) {
		return elements[row*M + col];
	}

	constexpr const float& operator()(const int row, const int col) const {
		return elements[row*M + col];
	}

	constexpr Matrix<N, M> operator+(const Matrix<N, M> &second) const {
		Matrix<N, M> result = {0};
		for(int i=0; i<N*M; i++)
			result.elements[i] = elements[i] + second.elements[i];
		return result;
	}

	constexpr void operator+=(const Matrix<N, M> &second) {
		for(int i=0; i<N*M; i++)
			elements[i] +=second.elements[i];
	}

	constexpr Matrix<N, M> operator-(const Matrix<N, M> &second) const {
		Matrix<N, M> result = {0};
		for(int i=0; i<N*M; i++)
			result.elements[i] = elements[i] - second.elements[i];
		return result;
	}

	template<int K>
	constexpr Matrix<N, K> operator*(const Matrix<M, K> &second) const {
		Matrix<N, K> result = {0};

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
		Matrix<M, N> result = {0};

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

	constexpr Matrix<N, N> inverse() const {

		Matrix<N, 2*N> combined = {0};

		for(int i=0; i<N; i++) {
			for(int j=0; j<N; j++)
				combined(i, j) = (*this)(i, j);
			combined(i, i+N) = 1;
		}

		for(int i=0; i<N; i++) {
			/*T maxVal = 0.001;
			int maxRow = i;

			for(int j=i; j<N; j++) {
				if(std::abs(combined(j, i))>maxVal) {
					maxVal = std::abs(combined(j, i));
					maxRow = j;
				}
			}

			if(maxVal<0.002)
				return {0};

			combined.swapRows(maxRow, i);*/

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

		Matrix<N, N> inverse = {0};

		for(int i=0; i<N; i++)
			for(int j=0; j<N; j++)
				inverse(i, j) = combined(i, j+N);

		return inverse;
	}
};

template<int N, int M>
constexpr Matrix<N, M> operator*(const Matrix<N, M> &matrix, const float &number) {
	Matrix<N, M> result = {0};
	for(int i=0; i<N; i++)
		for(int j=0; j<M; j++)
			result(i, j) = matrix(i, j)*number;
	return result;
}

template<int N, int M>
std::ostream& operator<<(std::ostream &stream, const Matrix<N, M> &matrix) {
	const std::ios::fmtflags flags = stream.flags();

	stream << std::setprecision(4) << std::fixed << std::showpos;

	for(int i=0; i<N; i++) {
		stream << std::setw(0);
		for(int j=0; j<M; j++)
			stream << matrix(i, j) << std::setw(15);
		stream << std::endl;
	}

	stream.flags(flags);

	return stream;
}
