/**
	Sparse matrix implementation on singly linked list

	Author: Belousov K.
	Repository: https://github.com/kombuchamp/SparseMatrices
*/

#pragma once
#include <iostream>
#include <exception>
#include <algorithm>
#include <map>
#include <utility>
#include <type_traits>
#include "ISparseMatrix.h"
#include "LinkedList.h"

template<typename T = double>
class LLSparseMatrix : ISparseMatrix<T>
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
	~LLSparseMatrix() = default;
	T ElementAt(int row, int col) const override;
	void Resize(int rows, int cols) override;
	void SetElement(int row, int col, T val) override;
	void RemoveElement(int row, int col) override;
	void Print(std::ostream &) const override;
	void Transpose() override;
	[[nodiscard]] size_t GetNonZeroElementsCount() const override;
	[[nodiscard]] size_t GetRowCount() const override;
	[[nodiscard]] size_t GetColCount() const override;
	LLSparseMatrix<T> *Multiply(LLSparseMatrix<T> &other);
	[[deprecated("Use multiply instead (more efficient)")]]
	LLSparseMatrix<T> *Multiply_DEPRECATED(LLSparseMatrix<T> *other);
	LLSparseMatrix<T> *operator*(LLSparseMatrix<T> &other);
private:
	struct MatrixNode;
	[[nodiscard]] bool InBoundaries(int row, int col) const;
	[[nodiscard]] int GetPosition(int row, int col) const;
	void SortByPosition();
	void MergeSort(MatrixNode **head);
	void SplitList(MatrixNode *head, MatrixNode **first, MatrixNode **second);
	MatrixNode *MergeLists(MatrixNode *list1, MatrixNode *list2);
	size_t _rowCount;
	size_t _colCount;
	LinkedList<MatrixNode> _nonZeroElements;
};

template<typename T>
struct LLSparseMatrix<T>::MatrixNode
{
	MatrixNode() = default;
	MatrixNode(const int row, const int col, T const &val)
		: Row(row), Col(col), Value(val)/*, Next(nullptr)*/
	{
	}
	MatrixNode &operator=(MatrixNode const &node) = default;
	int Row;
	int Col;
	T Value;
};

template<typename T>
void LLSparseMatrix<T>::Resize(const int rows, const int cols)
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
	for (auto elem : _nonZeroElements)
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
	if (_nonZeroElements.IsEmpty())
	{
		_nonZeroElements.AddLast(MatrixNode(row, col, val));
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
			_nonZeroElements.AddBefore(elemIt, MatrixNode(row, col, val));
			return;
		}
	}
	_nonZeroElements.AddLast(MatrixNode(row, col, val));
}

template<typename T>
void LLSparseMatrix<T>::RemoveElement(int row, int col)
{
	if (!InBoundaries(row, col))
	{
		throw std::invalid_argument("Element indices are out of bounds");
	}

	_nonZeroElements.RemoveIf(
		[=](auto &elem)
		{
			return elem.Row == row && elem.Col == col;
		});
}


template<typename T>
void LLSparseMatrix<T>::Print(std::ostream &os) const
{
	auto it = _nonZeroElements.begin();
	for (auto i = 0; i < _rowCount; i++)
	{
		for (auto j = 0; j < _colCount; j++)
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
	return _nonZeroElements.Count();
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
	_nonZeroElements.Sort(
		[this](MatrixNode &first, MatrixNode &second)
		{
			return GetPosition(first.Row, first.Col) < GetPosition(second.Row, second.Col);
		});
}

template<typename T>
LLSparseMatrix<T> *LLSparseMatrix<T>::Multiply(LLSparseMatrix<T> &other)
{
	if (this->_colCount != other._rowCount)
	{
		throw std::invalid_argument("Invalid argument: impossible to multiply incompatible matrices");
	}

	auto *result = new LLSparseMatrix(this->_rowCount, other._colCount);
	if (this->_nonZeroElements.IsEmpty() || other._nonZeroElements.IsEmpty())
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
		result->SetElement(i, j, value);
	}
	return result;
}

template<typename T>
bool LLSparseMatrix<T>::InBoundaries(const int row, const int col) const
{
	return (row < _rowCount && row >= 0) && (col < _colCount && col >= 0);
}

template<typename T>
int LLSparseMatrix<T>::GetPosition(const int row, const int col) const
{
	return _colCount * row + col;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, LLSparseMatrix<T> &mat)
{
	mat.Print(os);
	return os;
}

template<typename T>
LLSparseMatrix<T> *LLSparseMatrix<T>::operator*(LLSparseMatrix<T> &other)
{
	return Multiply(other);
}