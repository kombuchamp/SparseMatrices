/**
	Sparse matrix inerface

	Author: Belousov K.
*/

#pragma once
#include <ostream>

template<class T>
class ISparseMatrix
{
public:
	virtual ~ISparseMatrix() = default;
	virtual void SetElement(int row, int col, T val) = 0;
	virtual bool RemoveElement(int row, int col) = 0;
	[[nodiscard]] virtual T ElementAt(int row, int col) const = 0;
	virtual void Resize(int rows, int cols) = 0;
	virtual void Print(std::ostream &) const = 0;
	virtual void Transpose() = 0;
	[[nodiscard]] virtual int GetNonZeroElementsCount() const = 0;
	[[nodiscard]] virtual int GetRowCount() const = 0;
	[[nodiscard]] virtual int GetColCount() const = 0;
	// TODO: Come up with common multiplication interface
};