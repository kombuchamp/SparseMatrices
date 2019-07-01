/**
	Sparse matrix interface

	Author: Belousov K.
	Repository: https://github.com/kombuchamp/SparseMatrices
*/

#pragma once
#include <ostream>

template<class T>
class ISparseMatrix
{
public:
	virtual ~ISparseMatrix() = default;
	virtual void SetElement(int row, int col, T val) = 0;
	virtual void RemoveElement(int row, int col) = 0;
	[[nodiscard]] virtual T ElementAt(int row, int col) const = 0;
	virtual void Resize(const size_t rows, const size_t cols) = 0;
	virtual void Print(std::ostream &) const = 0;
	virtual void Transpose() = 0;
	[[nodiscard]] virtual size_t GetNonZeroElementsCount() const = 0;
	[[nodiscard]] virtual size_t GetRowCount() const = 0;
	[[nodiscard]] virtual size_t GetColCount() const = 0;
	// TODO: Come up with common multiplication interface
};