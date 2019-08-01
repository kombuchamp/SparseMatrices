/**
	Sparse matrix implementation on singly linked list

	Author: Belousov K.
	Repository: https://github.com/kombuchamp/SparseMatrices
*/

#pragma once
#include <exception>
#include <algorithm>
#include <map>
#include <list>
#include <utility>
#include <type_traits>
#include "ISparseMatrix.h"
#include "MatrixNode.h"

template<typename T = double>
class LLSparseMatrix
{
public:
	LLSparseMatrix()
		: LLSparseMatrix(0, 0)
	{
	}
	LLSparseMatrix(const int rows, const int cols)
		: _rowCount(rows), _colCount(cols)
	{
		static_assert(std::is_default_constructible<T>::value, "Template type T should have default constructor");
	}
	T ElementAt(int row, int col) const;
	void Resize(size_t rows, size_t cols);
	void SetElement(int row, int col, T val);
	void RemoveElement(int row, int col);
	void Print(std::ostream &) const;
	void Transpose();
	[[nodiscard]] size_t GetNonZeroElementsCount() const;
	[[nodiscard]] size_t GetRowCount() const;
	[[nodiscard]] size_t GetColCount() const;
	LLSparseMatrix<T> Multiply(LLSparseMatrix<T>& other);
private:
	[[nodiscard]] bool InBoundaries(size_t row, size_t col) const;
	[[nodiscard]] int GetPosition(size_t row, size_t col) const;
	size_t _rowCount;
	size_t _colCount;
	std::list<MatrixNode<T>> _nonZeroElements;
};

template<typename T>
void LLSparseMatrix<T>::Resize(const size_t rows, const size_t cols)
{
	if (rows < _rowCount || cols < _colCount)
	{
		throw std::invalid_argument("Can't reduce matrix size");
	}
	_rowCount = rows;
	_colCount = cols;
}

template<typename T>
T LLSparseMatrix<T>::ElementAt(int row, int col) const
{
	if (!InBoundaries(row, col))
	{
		throw std::invalid_argument("Element indices are out of bounds");
	}
	for (auto &elem : _nonZeroElements)
	{
		if (elem.Row == row && elem.Col == col)
		{
			return elem.Value;
		}
	}
	return T();
}

template<typename T>
void LLSparseMatrix<T>::SetElement(int row, int col, T val)
{
	if (!InBoundaries(row, col))
	{
		throw std::invalid_argument("Element indices are out of bounds");
	}
	if (val == T())
	{
		return;
	}
	if (_nonZeroElements.empty())
	{
		_nonZeroElements.emplace_back(MatrixNode(row, col, val));
		return;
	}
	const auto newElementPosition = GetPosition(row, col);
	for (auto elemIt = _nonZeroElements.begin(); elemIt != _nonZeroElements.end(); ++elemIt)
	{
		auto currentElementPosition = GetPosition(elemIt->Row, elemIt->Col);
		if (newElementPosition == currentElementPosition)
		{
			elemIt->Value = val;
			return;
		}
		if (newElementPosition < currentElementPosition)
		{
			_nonZeroElements.insert(elemIt, MatrixNode(row, col, val));
			return;
		}
	}
	_nonZeroElements.emplace_back(MatrixNode(row, col, val));
}

template<typename T>
void LLSparseMatrix<T>::RemoveElement(int row, int col)
{
	if (!InBoundaries(row, col))
	{
		throw std::invalid_argument("Element indices are out of bounds");
	}

	_nonZeroElements.remove_if(
		[=](auto &elem)
		{
			return elem.Row == row && elem.Col == col;
		});
}


template<typename T>
void LLSparseMatrix<T>::Print(std::ostream &os) const
{
	auto it = _nonZeroElements.begin();
	for (size_t i = 0; i < _rowCount; i++)
	{
		for (size_t j = 0; j < _colCount; j++)
		{
			if (it != _nonZeroElements.end() && it->Row == i && it->Col == j)
			{
				os << it->Value << " ";
				++it;
			}
			else
			{
				os << T() << " ";
			}
		}
		os << std::endl;
	}
}

template<typename T>
size_t LLSparseMatrix<T>::GetNonZeroElementsCount() const
{
	return _nonZeroElements.size();
}

template<typename T>
size_t LLSparseMatrix<T>::GetRowCount() const
{
	return _rowCount;
}

template<typename T>
size_t LLSparseMatrix<T>::GetColCount() const
{
	return _colCount;
}


template<typename T>
void LLSparseMatrix<T>::Transpose()
{
	for (auto &elem : _nonZeroElements)
	{
		std::swap(elem.Row, elem.Col);
	}
	std::swap(_rowCount, _colCount);
	_nonZeroElements.sort(
		[this](auto &first, auto &second)
		{
			return GetPosition(first.Row, first.Col) < GetPosition(second.Row, second.Col);
		});
}

template<typename T>
LLSparseMatrix<T> LLSparseMatrix<T>::Multiply(LLSparseMatrix<T>& other)
{
	if (this->_colCount != other._rowCount)
	{
		throw std::invalid_argument("Invalid argument: impossible to multiply incompatible matrices");
	}

	LLSparseMatrix result(this->_rowCount, other._colCount);
	if (this->_nonZeroElements.empty() || other._nonZeroElements.empty())
	{
		return result;
	}
	auto thisIt = this->_nonZeroElements.begin();
	auto otherIt = other._nonZeroElements.begin();
	std::map<std::pair<int, int>, T> idxValMap;

	// Multiplication loop
	/**
	 * Iterate through first matrix elements A[i, j].
	 * Multiply each one by every element in j-th Row of other matrix.
	 * Accumulate multiplication result in idxValMap
	 * where key is pair of indices of element in resulting matrix.
	 * This algorithm allows us to avoid matrix transposition or picking out column during multiplication
	 */

	while (thisIt != this->_nonZeroElements.end())
	{
		otherIt = other._nonZeroElements.begin();

		// Find corresponding row
		if (thisIt->Col != otherIt->Row)
		{
			while (otherIt != other._nonZeroElements.end() && thisIt->Col != otherIt->Row)
			{
				++otherIt;
			}
			if (otherIt == other._nonZeroElements.end())
			{
				++thisIt;
				continue;
			}
		}

		// Calculate partial sums
		while (otherIt != other._nonZeroElements.end() && thisIt->Col == otherIt->Row)
		{
			int i = thisIt->Row;
			int j = otherIt->Col;
			idxValMap[std::pair<int, int>(i, j)] += thisIt->Value * otherIt->Value;

			++otherIt;
		}
		++thisIt;
	}

	for (auto item : idxValMap)
	{
		auto [indices, value] = item;
		auto [i, j] = indices;
		result.SetElement(i, j, value);
	}
	return result;
}

template<typename T>
bool LLSparseMatrix<T>::InBoundaries(const size_t row, const size_t col) const
{
	return (row < _rowCount && row >= 0) && (col < _colCount && col >= 0);
}

template<typename T>
int LLSparseMatrix<T>::GetPosition(const size_t row, const size_t col) const
{
	return _colCount * row + col;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, LLSparseMatrix<T> &mat)
{
	mat.Print(os);
	return os;
}
