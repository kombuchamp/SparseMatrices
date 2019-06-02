#pragma once
#include <ostream>

template<class T>
class ISparseMatrix
{
public:
	virtual void SetElement(int row, int col, T val) = 0;
	virtual bool RemoveElement(int row, int col) = 0;
	virtual T ElementAt(int row, int col) const = 0;
	virtual void Resize(int rows, int cols) = 0;
	virtual void Print(std::ostream &) const = 0;
	virtual void Transponse() = 0;
};