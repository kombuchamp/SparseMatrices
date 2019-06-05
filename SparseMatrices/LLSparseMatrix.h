/**
	Sparse matrix implementation on singly linked list

	Author: Belousov K.
*/

#pragma once
#include <iostream>
#include <exception>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include "ISparseMatrix.h"

template<class T = double> // Type with default ctor expected
class LLSparseMatrix : ISparseMatrix<T>
{
public:
	LLSparseMatrix() = default;
	LLSparseMatrix(const int rows, const int cols)
		: rowCount(rows), colCount(cols), nonZeroElementsCount(0), firstNode(nullptr)
	{
	}
	~LLSparseMatrix();
	T ElementAt(int row, int col) const override;
	void Resize(int rows, int cols) override;
	void SetElement(int row, int col, T val) override;
	bool RemoveElement(int row, int col) override;
	void Print(std::ostream &) const override;
	void Transpose() override;
	[[nodiscard]] int GetNonZeroElementsCount() const override;
	[[nodiscard]] int GetRowCount() const override;
	[[nodiscard]] int GetColCount() const override;
	LLSparseMatrix<T> *Multiply(LLSparseMatrix<T> *other);
	[[deprecated("Use multiply instead (more efficient)")]]
	LLSparseMatrix<T> *Multiply_DEPRECATED(LLSparseMatrix<T> *other);
	LLSparseMatrix<T>* operator*(LLSparseMatrix<T>* other);
private:
	struct MatrixNode;
	[[nodiscard]] bool InBoundaries(int row, int col) const;
	[[nodiscard]] int GetPosition(int row, int col) const;
	void SortByPosition(MatrixNode **head);
	void SplitList(MatrixNode *head, MatrixNode **first, MatrixNode **second);
	MatrixNode *MergeLists(MatrixNode *list1, MatrixNode *list2);
	int rowCount;
	int colCount;
	int nonZeroElementsCount;
	MatrixNode *firstNode;
};

template<class T>
struct LLSparseMatrix<T>::MatrixNode
{
	MatrixNode(const int row, const int col, T const &val)
		: row(row), col(col), value(val), nextNode(nullptr)
	{
	}
	int row;
	int col;
	T value;
	MatrixNode *nextNode;
};


template<class T>
void LLSparseMatrix<T>::Resize(const int rows, const int cols)
{
	if (rows < rowCount || cols < colCount)
	{
		throw std::invalid_argument("Can't reduce matrix size");
	}
	rowCount = rows;
	colCount = cols;
}

template<class T>
LLSparseMatrix<T>::~LLSparseMatrix()
{
	if (firstNode == nullptr)
	{
		return;
	}
	if (firstNode->nextNode == nullptr)
	{
		delete firstNode;
		return;
	}
	MatrixNode *prevNode = firstNode;
	for (MatrixNode *node = firstNode->nextNode; node != nullptr; node = node->nextNode)
	{
		delete prevNode;
		prevNode = node;
	}
}

template<class T>
T LLSparseMatrix<T>::ElementAt(int row, int col) const
{
	if (!InBoundaries(row, col))
	{
		throw std::invalid_argument("Element indices are out of bounds");
	}
	for (auto *node = firstNode; node != nullptr; node = node->nextNode)
	{
		if (node->row == row && node->col == col)
		{
			return node->value;
		}
	}
	return T();
}

template<class T>
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
	++nonZeroElementsCount;
	if (firstNode == nullptr)
	{
		firstNode = new MatrixNode(row, col, val);
	}
	else
	{
		auto position = GetPosition(row, col);
		MatrixNode *prevNode = nullptr;
		for (auto node = firstNode; node != nullptr; node = node->nextNode)
		{
			if (node->row == row && node->col == col)
			{
				node->value = val;
				return;
			}
			auto currentPosition = GetPosition(node->row, node->col);
			if (position < currentPosition)
			{
				auto *newNode = new MatrixNode(row, col, val);
				if (prevNode == nullptr) // We have first and only node
				{
					newNode->nextNode = firstNode;
					firstNode = newNode;
					return;
				}
				prevNode->nextNode = newNode;
				newNode->nextNode = node;
				return;
			}
			prevNode = node;
		}
		auto *newNode = new MatrixNode(row, col, val);
		prevNode->nextNode = newNode;
	}
}

template<class T>
bool LLSparseMatrix<T>::RemoveElement(int row, int col)
{
	if (!InBoundaries(row, col))
	{
		throw std::invalid_argument("Element indices are out of bounds");
	}

	MatrixNode *prevNode = nullptr;
	for (MatrixNode *node = firstNode; node != nullptr; node = node->nextNode)
	{
		if (node->row == row && node->col == col)
		{
			if (node == firstNode)
			{
				firstNode = node->nextNode;
				delete node;
			}
			else
			{
				prevNode->nextNode = node->nextNode;
				delete node;
			}
			--nonZeroElementsCount;
			return true;
		}
		prevNode = node;
	}
	return false;
}


template<class T>
void LLSparseMatrix<T>::Print(std::ostream &os) const
{
	auto *node = firstNode;
	for (auto i = 0; i < rowCount; i++)
	{
		for (auto j = 0; j < colCount; j++)
		{
			if (node && node->row == i && node->col == j)
			{
				os << node->value << " ";
				node = node->nextNode;
			}
			else
			{
				os << T() << " ";
			}
		}
		os << std::endl;
	}
}

template<class T>
int LLSparseMatrix<T>::GetNonZeroElementsCount() const
{
	return nonZeroElementsCount;
}

template<class T>
int LLSparseMatrix<T>::GetRowCount() const
{
	return rowCount;
}

template<class T>
int LLSparseMatrix<T>::GetColCount() const
{
	return colCount;
}


template<class T>
void LLSparseMatrix<T>::Transpose()
{
	for (MatrixNode *node = firstNode; node != nullptr; node = node->nextNode)
	{
		std::swap(node->row, node->col);
	}
	std::swap(rowCount, colCount);
	SortByPosition(&firstNode);
}

/**
 * Naive implementation of matrix multiplication
 * This algorithm is inefficient, ugly, non-robust and the only reason I'm keeping it here
 * being amounts of blood I spilled figuring it out.
 * I came up with something more efficient, see Multiply method
 */
template<class T>
LLSparseMatrix<T> *LLSparseMatrix<T>::Multiply_DEPRECATED(LLSparseMatrix<T> *other)
{
	if (this->colCount != other->rowCount)
	{
		throw std::invalid_argument("Invalid argument: impossible to multiply incompatible matrices");
	}
	auto *result = new LLSparseMatrix(this->rowCount, other->colCount);
	other->Transpose();
	auto *thisItr = this->firstNode;
	auto *otherItr = other->firstNode;
	std::map< std::pair<int, int>, T> idxValMap;
	auto *currentRowStart = thisItr;
	bool isLastRow = false;

	while (true)
	{
		if (thisItr == nullptr)
		{
			// Last row ended
			if (otherItr == nullptr || otherItr->nextNode == nullptr)
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
			while (!(currentRowStart->row < thisItr->row))
			{
				if (thisItr->nextNode == nullptr)
				{
					// On the last row. Done
					isLastRow = true;
					break;
				}
				thisItr = thisItr->nextNode;
			}
			if (isLastRow)
			{
				// Done
				break;
			}
			currentRowStart = thisItr;
			otherItr = other->firstNode;
		}
		if (thisItr->row != currentRowStart->row)
		{
			// We slipped over current row, get back
			thisItr = currentRowStart;
		}

		if (thisItr->col == otherItr->col)
		{
			int i = thisItr->row;
			int j = otherItr->row;
			idxValMap[std::pair<int, int>(i, j)] += thisItr->value * otherItr->value;


			// Move both itrs if they both will be on the same line next turn
			if (thisItr->nextNode == nullptr
				|| otherItr->nextNode == nullptr
				|| thisItr->nextNode->row == otherItr->nextNode->row)
			{
				// If other row changes here, we can return first row to the beginning of line to prevent everything from breaking
				if (otherItr->nextNode != nullptr && otherItr->row != otherItr->nextNode->row)
				{
					thisItr = currentRowStart;
				}
				else
				{
					thisItr = thisItr->nextNode;
				}
				otherItr = otherItr->nextNode;
			}
			else if (thisItr->nextNode->row > otherItr->nextNode->row)
			{
				otherItr = otherItr->nextNode;
			}
			else if (thisItr->nextNode->row < otherItr->nextNode->row)
			{
				thisItr = thisItr->nextNode;
			}
		}
		else if (thisItr->col < otherItr->col)
		{
			// If thisItr stepped into another row, return otherItr to its beginning
			if (thisItr->nextNode != nullptr && thisItr->row != thisItr->nextNode->row)
			{
				otherItr = otherItr->nextNode;
			}
			thisItr = thisItr->nextNode;
		}
		else //(thisItr->col > otherItr->col)
		{
			// If otherItr stepped into another row, return thisItr to its beginning
			if (otherItr->nextNode != nullptr && otherItr->row != otherItr->nextNode->row)
			{
				thisItr = currentRowStart;
			}
			otherItr = otherItr->nextNode;
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

template<class T>
LLSparseMatrix<T> *LLSparseMatrix<T>::Multiply(LLSparseMatrix<T> *other)
{
	if (other == nullptr)
	{
		throw std::invalid_argument("Other matrix can't be nullptr");
	}
	if (this->colCount != other->rowCount)
	{
		throw std::invalid_argument("Invalid argument: impossible to multiply incompatible matrices");
	}

	auto *result = new LLSparseMatrix(this->rowCount, other->colCount);
	if (this->firstNode == nullptr || other->firstNode == nullptr)
	{
		return result;
	}

	auto *thisPtr = this->firstNode;
	auto *otherPtr = other->firstNode;
	std::map<std::pair<int, int>, T> idxValMap;

	// Multiplication loop
	/**
	 * Iterate through first matrix elements A[i, j].
	 * Multiply each one by every element in j-th row of other matrix.
	 * Accumulate multiplication result in idxValMap
	 * where key is pair of indices of element in resulting matrix.
	 * This algorithm allows us to avoid matrix transposition or picking out column during multiplication
	 */
	while (thisPtr != nullptr)
	{
		// Just reset.
		// Can't just remember previous row because of sparsity
		otherPtr = other->firstNode;

		// Find corresponding row
		if (thisPtr->col != otherPtr->row)
		{
			while (otherPtr != nullptr && thisPtr->col != otherPtr->row)
			{
				otherPtr = otherPtr->nextNode;
			}
			if (otherPtr == nullptr)
			{
				thisPtr = thisPtr->nextNode;
				continue;
			}
		}

		// Calculate partial sums
		while (otherPtr != nullptr && thisPtr->col == otherPtr->row)
		{
			int i = thisPtr->row;
			int j = otherPtr->col;
			idxValMap[std::pair<int, int>(i, j)] += thisPtr->value * otherPtr->value;

			otherPtr = otherPtr->nextNode;
		}

		thisPtr = thisPtr->nextNode;
	}

	for (auto item : idxValMap)
	{
		auto [indices, value] = item;
		auto [i, j] = indices;
		result->SetElement(i, j, value);
	}
	return result;
}

template<class T>
void LLSparseMatrix<T>::SortByPosition(MatrixNode **head)
{
	MatrixNode *currentHead = *head;
	MatrixNode *split1, *split2;
	if (currentHead == nullptr || currentHead->nextNode == nullptr)
	{
		return;
	}
	SplitList(currentHead, &split1, &split2);
	SortByPosition(&split1);
	SortByPosition(&split2);
	*head = MergeLists(split1, split2);
}

template<class T>
void LLSparseMatrix<T>::SplitList(MatrixNode *head, MatrixNode **first, MatrixNode **second)
{
	// Floyd's tortoise algorithm of finding middle of linked list
	MatrixNode *slow = head;
	MatrixNode *fast = head->nextNode;

	while (fast != nullptr)
	{
		fast = fast->nextNode;
		if (fast != nullptr)
		{
			slow = slow->nextNode;
			fast = fast->nextNode;
		}
	}

	*first = head;
	*second = slow->nextNode;
	slow->nextNode = nullptr; // Tear list apart
}

template<class T>
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
	auto firstPosition = GetPosition(list1->row, list1->col);
	auto secondPosition = GetPosition(list2->row, list2->col);
	if (firstPosition <= secondPosition)
	{
		newHead = list1;
		newHead->nextNode = MergeLists(list1->nextNode, list2);
	}
	else
	{
		newHead = list2;
		newHead->nextNode = MergeLists(list1, list2->nextNode);
	}
	return newHead;
}

template<class T>
bool LLSparseMatrix<T>::InBoundaries(const int row, const int col) const
{
	return (row < rowCount && row >= 0) && (col < colCount && col >= 0);
}

template<class T>
int LLSparseMatrix<T>::GetPosition(const int row, const int col) const
{
	return colCount * row + col;
}

template<class T>
std::ostream &operator<<(std::ostream &os, LLSparseMatrix<T> &sm)
{
	sm.Print(os);
	return os;
}

template<class T>
LLSparseMatrix<T> *LLSparseMatrix<T>::operator*(LLSparseMatrix<T> *other)
{
	return Multiply(other);
}