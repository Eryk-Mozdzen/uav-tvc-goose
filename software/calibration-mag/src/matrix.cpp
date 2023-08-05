#include "matrix.h"
#include <cmath>
#include <cassert>

Matrix::Matrix(int N, int M) : N{N}, M{M} {
	elements.resize(N*M);

	for(float &elem : elements) {
		elem = 0.f;
	}
}

void Matrix::append(const std::vector<float> row) {
	assert(static_cast<int>(row.size())==M);

	elements.insert(elements.end(), row.begin(), row.end());
	N++;
}

int Matrix::getRows() const {
	return N;
}

int Matrix::getCols() const {
	return M;
}

float& Matrix::operator()(const int row, const int col) {
	return elements[row*M + col];
}

const float& Matrix::operator()(const int row, const int col) const {
	return elements[row*M + col];
}

Matrix Matrix::operator+(const Matrix &second) const {
	assert(N==second.N && M==second.M);

	Matrix result(N, M);
	for(int i=0; i<N*M; i++)
		result.elements[i] = elements[i] + second.elements[i];
	return result;
}

void Matrix::operator+=(const Matrix &second) {
	assert(N==second.N && M==second.M);

	for(int i=0; i<N*M; i++)
		elements[i] +=second.elements[i];
}

Matrix Matrix::operator-(const Matrix &second) const {
	assert(N==second.N && M==second.M);

	Matrix result(N, M);
	for(int i=0; i<N*M; i++)
		result.elements[i] = elements[i] - second.elements[i];
	return result;
}

Matrix Matrix::operator*(const Matrix &second) const {
	assert(M==second.N);

	const int K = second.M;

	Matrix result(N, K);

	for(int i=0; i<N; i++)
		for(int j=0; j<K; j++)
			for(int k=0; k<M; k++)
				result(i, j) +=(*this)(i, k)*second(k, j);

	return result;
}

Matrix Matrix::operator*(const float &number) const {
	Matrix result = *this;
	for(int i=0; i<N*M; i++)
		result.elements[i] = elements[i]*number;
	return result;
}

Matrix Matrix::operator/(const float &number) const {
	Matrix result = *this;
	for(int i=0; i<N*M; i++)
		result.elements[i] = elements[i]/number;
	return result;
}

Matrix Matrix::transposition() const {
	Matrix result(M, N);

	for(int i=0; i<N; i++)
		for(int j=0; j<M; j++)
			result(j, i) = (*this)(i, j);

	return result;
}

void Matrix::swapRows(const int row1, const int row2) {
	for(int i=0; i<M; i++) {
		const float tmp = (*this)(row1, i);
		(*this)(row1, i) = (*this)(row2, i);
		(*this)(row2, i) = tmp;
	}
}

Matrix Matrix::identity(const int N) {
	Matrix result(N, N);

	for(int i=0; i<N; i++)
		result(i, i) = 1;

	return result;
}

Matrix Matrix::inverse() const {
	assert(N==M);

	if(N==1) {
		if(elements[0]<0.002f) {
			std::cerr << "can't inverse 1x1 matrix" << std::endl;
			return Matrix(1, 1);
		}

		Matrix inv(1, 1);
		inv(0, 0) = 1.f/elements[0];
		return inv;
	}

	Matrix combined(N, 2*N);

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
			std::cerr << "can't inverse " << N << "x" << N << " matrix" << std::endl;
			return Matrix(N, N);
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

	Matrix inverse(N, N);

	for(int i=0; i<N; i++)
		for(int j=0; j<N; j++)
			inverse(i, j) = combined(i, j+N);

	return inverse;
}

std::ostream & operator<<(std::ostream &stream, const Matrix &matrix) {
	const int N = matrix.getRows();
	const int M = matrix.getCols();

	for(int i=0; i<N; i++) {
		stream << "[";
		for(int j=0; j<M; j++) {
			stream << matrix(i, j);
			if(j!=M-1) {
				stream << ", ";
			}
		}
		stream << "]";
		if(i!=N-1) {
			stream << ", ";
		}
	}

	return stream;
}
