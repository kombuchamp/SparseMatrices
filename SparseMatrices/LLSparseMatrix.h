/**
	Sparse matrix implementation on singly linked list

	Author: Belousov K.
	Repository: https://github.com/kombuchamp/SparseMatrices
*/

#pragma once
#include <iostream>
#include <exception>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <type_traits>
#include "ISparseMatrix.h"

template<typename T = double>
class LLSparseMatrix : ISparseMatrix<T>
{
public:
	LLSparseMatrix()
		: LLSparseMatrix(0, 0) { }
	LLSparseMatrix(const int rows, const int cols)
		: _rowCount(rows), _colCount(cols), _nonZeroElementsCount(0), _firstNode(nullptr)
	{
		static_assert(std::is_default_constructible<T>::value, "Template type T should have default constructor");
	}
	~LLSparseMatrix();
	T ElementAt(int row, int col) const override;
	void Resize(int rows, int cols) override;
	void SetElement(int row, int col, T val) override;
	bool RemoveElement(int row, int col) override;
	void Print(std::ostream &) const override;
	void Transpose() override;
	[[nodiscard]] size_t GetNonZeroElementsCount() const override;
	[[nodiscard]] size_t GetRowCount() const override;
	[[nodiscard]] size_t GetColCount() const override;
	LLSparseMatrix<T> *Multiply(LLSparseMatrix<T>& other);
	[[deprecated("Use multiply instead (more efficient)")]]
	LLSparseMatrix<T> *Multiply_DEPRECATED(LLSparseMatrix<T> *other);
	LLSparseMatrix<T>* operator*(LLSparseMatrix<T> &other);
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
	size_t _nonZeroElementsCount;
	MatrixNode *_firstNode;
};

template<typename T>
struct LLSparseMatrix<T>::MatrixNode
{
	MatrixNode(const int row, const int col, T const &val)
		: Row(row), Col(col), Value(val), Next(nullptr)
	{
	}
	int Row;
	int Col;
	T Value;
	MatrixNode *Next;
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
LLSparseMatrix<T>::~LLSparseMatrix()
{
	if (_firstNode == nullptr)
	{
		return;
	}
	if (_firstNode->Next == nullptr)
	{
		delete _firstNode;
		return;
	}
	MatrixNode *prevNode = _firstNode;
	for (MatrixNode *node = _firstNode->Next; node != nullptr; node = node->Next)
	{
		delete prevNode;
		prevNode = node;
	}
}

template<typename T>
T LLSparseMatrix<T>::ElementAt(int row, int col) const
{
	if (!InBoundaries(row, col))
	{
		throw std::invalid_argument("Element indices are out of bounds");
	}
	for (auto *node = _firstNode; node != nullptr; node = node->Next)
	{
		if (node->Row == row && node->Col == col)
		{
			return node->Value;
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
	++_nonZeroElementsCount;
	if (_firstNode == nullptr)
	{
		_firstNode = new MatrixNode(row, col, val);
	}
	else
	{
		auto position = GetPosition(row, col);
		MatrixNode *prevNode = nullptr;
		for (auto node = _firstNode; node != nullptr; node = node->Next)
		{
			if (node->Row == row && node->Col == col)
			{
				node->Value = val;
				return;
			}
			auto currentPosition = GetPosition(node->Row, node->Col);
			if (position < currentPosition)
			{
				auto *newNode = new MatrixNode(row, col, val);
				if (prevNode == nullptr) // We have first and only node
				{
					newNode->Next = _firstNode;
					_firstNode = newNode;
					return;
				}
				prevNode->Next = newNode;
				newNode->Next = node;
				return;
			}
			prevNode = node;
		}
		auto *newNode = new MatrixNode(row, col, val);
		prevNode->Next = newNode;
	}
}

template<typename T>
bool LLSparseMatrix<T>::RemoveElement(int row, int col)
{
	if (!InBoundaries(row, col))
	{
		throw std::invalid_argument("Element indices are out of bounds");
	}

	MatrixNode *prevNode = nullptr;
	for (MatrixNode *node = _firstNode; node != nullptr; node = node->Next)
	{
		if (node->Row == row && node->Col == col)
		{
			if (node == _firstNode)
			{
				_firstNode = node->Next;
				delete node;
			}
			else
			{
				prevNode->Next = node->Next;
				delete node;
			}
			--_nonZeroElementsCount;
			return true;
		}
		prevNode = node;
	}
	return false;
}


template<typename T>
void LLSparseMatrix<T>::Print(std::ostream &os) const
{
	auto *node = _firstNode;
	for (auto i = 0; i < _rowCount; i++)
	{
		for (auto j = 0; j < _colCount; j++)
		{
			if (node && node->Row == i && node->Col == j)
			{
				os << node->Value << " ";
				node = node->Next;
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
	return _nonZeroElementsCount;
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
	for (MatrixNode *node = _firstNode; node != nullptr; node = node->Next)
	{
		std::swap(node->Row, node->Col);
	}
	std::swap(_rowCount, _colCount);
	SortByPosition();
}

/**
 * Naive implementation of matrix multiplication
 * This algorithm is inefficient, ugly, non-robust and the only reason I'm keeping it here
 * being amounts of blood I spilled figuring it out.
 * I came up with something more efficient, see Multiply method
 */
template<typename T>
LLSparseMatrix<T> *LLSparseMatrix<T>::Multiply_DEPRECATED(LLSparseMatrix<T> *other)
{
	if (this->_colCount != other->_rowCount)
	{
		throw std::invalid_argument("Invalid argument: impossible to multiply incompatible matrices");
	}
	auto *result = new LLSparseMatrix(this->_rowCount, other->_colCount);
	other->Transpose();
	auto *thisItr = this->_firstNode;
	auto *otherItr = other->_firstNode;
	std::map< std::pair<int, int>, T> idxValMap;
	auto *currentRowStart = thisItr;
	bool isLastRow = false;

	while (true)
	{
		if (thisItr == nullptr)
		{
			// Last Row ended
			if (otherItr == nullptr || otherItr->Next == nullptr)
			{
				// We done
				break;
			}
			isLastRow = true;
			thisItr = currentRowStart;
		}
		if (otherItr == nullptr)
		{
			// We can reach the end of other mat before the current mat
			while (!(currentRowStart->Row < thisItr->Row))
			{
				if (thisItr->Next == nullptr)
				{
					// On the last Row. Done
					isLastRow = true;
					break;
				}
				thisItr = thisItr->Next;
			}
			if (isLastRow)
			{
				// Done
				break;
			}
			currentRowStart = thisItr;
			otherItr = other->_firstNode;
		}
		if (thisItr->Row != currentRowStart->Row)
		{
			// We slipped over current Row, get back
			thisItr = currentRowStart;
		}

		if (thisItr->Col == otherItr->Col)
		{
			int i = thisItr->Row;
			int j = otherItr->Row;
			idxValMap[std::pair<int, int>(i, j)] += thisItr->Value * otherItr->Value;


			// Move both itrs if they both will be on the same line next turn
			if (thisItr->Next == nullptr
				|| otherItr->Next == nullptr
				|| thisItr->Next->Row == otherItr->Next->Row)
			{
				// If other Row changes here, we can return first Row to the beginning of line to prevent everything from breaking
				if (otherItr->Next != nullptr && otherItr->Row != otherItr->Next->Row)
				{
					thisItr = currentRowStart;
				}
				else
				{
					thisItr = thisItr->Next;
				}
				otherItr = otherItr->Next;
			}
			else if (thisItr->Next->Row > otherItr->Next->Row)
			{
				otherItr = otherItr->Next;
			}
			else if (thisItr->Next->Row < otherItr->Next->Row)
			{
				thisItr = thisItr->Next;
			}
		}
		else if (thisItr->Col < otherItr->Col)
		{
			// If thisItr stepped into another Row, return otherItr to its beginning
			if (thisItr->Next != nullptr && thisItr->Row != thisItr->Next->Row)
			{
				otherItr = otherItr->Next;
			}
			thisItr = thisItr->Next;
		}
		else //(thisItr->Col > otherItr->Col)
		{
			// If otherItr stepped into another Row, return thisItr to its beginning
			if (otherItr->Next != nullptr && otherItr->Row != otherItr->Next->Row)
			{
				thisItr = currentRowStart;
			}
			otherItr = otherItr->Next;
		}
	}
	other->Transpose();
	for (auto item : idxValMap)
	{
		auto [indices, value] = item;
		auto [i, j] = indices;
		result->SetElement(i, j, value);
	}
	return result;
}

template<typename T>
LLSparseMatrix<T> *LLSparseMatrix<T>::Multiply(LLSparseMatrix<T> &other)
{
	if (this->_colCount != other._rowCount)
	{
		throw std::invalid_argument("Invalid argument: impossible to multiply incompatible matrices");
	}

	auto *result = new LLSparseMatrix(this->_rowCount, other._colCount);
	if (this->_firstNode == nullptr || other._firstNode == nullptr)
	{
		return result;
	}

	auto *thisPtr = this->_firstNode;
	auto *otherPtr = other._firstNode;
	std::map<std::pair<int, int>, T> idxValMap;

	// Multiplication loop
	/**
	 * Iterate through first matrix elements A[i, j].
	 * Multiply each one by every element in j-th Row of other matrix.
	 * Accumulate multiplication result in idxValMap
	 * where key is pair of indices of element in resulting matrix.
	 * This algorithm allows us to avoid matrix transposition or picking out column during multiplication
	 */
	while (thisPtr != nullptr)
	{
		// Just reset.
		// Can't just remember previous Row because of sparsity
		otherPtr = other._firstNode;

		// Find corresponding Row
		if (thisPtr->Col != otherPtr->Row)
		{
			while (otherPtr != nullptr && thisPtr->Col != otherPtr->Row)
			{
				otherPtr = otherPtr->Next;
			}
			if (otherPtr == nullptr)
			{
				thisPtr = thisPtr->Next;
				continue;
			}
		}

		// Calculate partial sums
		while (otherPtr != nullptr && thisPtr->Col == otherPtr->Row)
		{
			int i = thisPtr->Row;
			int j = otherPtr->Col;
			idxValMap[std::pair<int, int>(i, j)] += thisPtr->Value * otherPtr->Value;

			otherPtr = otherPtr->Next;
		}

		thisPtr = thisPtr->Next;
	}

	for (auto item : idxValMap)
	{
		auto [indices, value] = item;
		auto [i, j] = indices;
		result->SetElement(i, j, value);
	}
	return result;
}

template <typename T>
void LLSparseMatrix<T>::SortByPosition()
{
	// TODO: Create a namespace and move sorting function out of the class?
	MergeSort(&_firstNode);
}

template<typename T>
void LLSparseMatrix<T>::MergeSort(MatrixNode **head)
{
	MatrixNode *currentHead = *head;
	MatrixNode *split1, *split2;
	if (currentHead == nullptr || currentHead->Next == nullptr)
	{
		return;
	}
	SplitList(currentHead, &split1, &split2);
	MergeSort(&split1);
	MergeSort(&split2);
	*head = MergeLists(split1, split2);
}

template<typename T>
void LLSparseMatrix<T>::SplitList(MatrixNode *head, MatrixNode **first, MatrixNode **second)
{
	// Floyd's tortoise algorithm of finding middle of linked list
	MatrixNode *slow = head;
	MatrixNode *fast = head->Next;

	while (fast != nullptr)
	{
		fast = fast->Next;
		if (fast != nullptr)
		{
			slow = slow->Next;
			fast = fast->Next;
		}
	}

	*first = head;
	*second = slow->Next;
	slow->Next = nullptr; // Tear list apart
}

template<typename T>
typename LLSparseMatrix<T>::MatrixNode *LLSparseMatrix<T>::MergeLists(MatrixNode *list1, MatrixNode *list2)
{
	MatrixNode *newHead;
	if (list1 == nullptr)
	{
		return list2;
	}
	if (list2 == nullptr)
	{
		return list1;
	}
	auto firstPosition = GetPosition(list1->Row, list1->Col);
	auto secondPosition = GetPosition(list2->Row, list2->Col);
	if (firstPosition <= secondPosition)
	{
		newHead = list1;
		newHead->Next = MergeLists(list1->Next, list2);
	}
	else
	{
		newHead = list2;
		newHead->Next = MergeLists(list1, list2->Next);
	}
	return newHead;
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