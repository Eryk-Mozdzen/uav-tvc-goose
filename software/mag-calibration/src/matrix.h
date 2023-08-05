#pragma once

#include <vector>
#include <iostream>

class Matrix {
	int N, M;
	std::vector<float> elements;

public:

	Matrix(int N, int M);
	void append(const std::vector<float> row);
	int getCols() const;
	int getRows() const;
	float& operator()(const int row, const int col);
	const float& operator()(const int row, const int col) const;
	Matrix operator+(const Matrix &second) const;
	void operator+=(const Matrix &second);
	Matrix operator-(const Matrix &second) const;
	Matrix operator*(const Matrix &second) const;
	Matrix operator*(const float &number) const;
	Matrix operator/(const float &number) const;
	Matrix transposition() const;
	void swapRows(const int row1, const int row2);
	static Matrix identity(const int N);
	Matrix inverse() const;
};

std::ostream & operator<<(std::ostream &stream, const Matrix &matrix);
