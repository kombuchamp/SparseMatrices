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
	LLSparseMatrix(int rows, int cols)
		: rowCount(rows), colCount(cols), firstNode(nullptr), nonZeroElementsCount(0)
	{
	}
	~LLSparseMatrix();
	T ElementAt(int row, int col) const;
	void Resize(int rows, int cols);
	void SetElement(int row, int col, T val);
	bool RemoveElement(int row, int col);
	void Print(std::ostream &) const;
	int GetNonZeroElementsCount() const;
	int GetRowCount() const;
	int GetColCount() const;
	void Transponse();
	LLSparseMatrix<T> *Multiply(LLSparseMatrix<T> *other);
private:
	struct MatrixNode;
	bool InBoundaries(int row, int col) const;
	int GetPosition(int i, int j);
	void SortByPosition(MatrixNode **head);
	void SplitList(MatrixNode *head, MatrixNode **first, MatrixNode **second);
	MatrixNode *MergeSortedLists(MatrixNode *list1, MatrixNode *list2);
	int rowCount;
	int colCount;
	int nonZeroElementsCount;
	MatrixNode *firstNode;
};

template<class T>
struct LLSparseMatrix<T>::MatrixNode
{
	MatrixNode(int row, int col, T &val)
		: row(row), col(col), value(val), nextNode(nullptr)
	{
	}
	int row;
	int col;
	T value;
	MatrixNode *nextNode;
};


template<class T>
void LLSparseMatrix<T>::Resize(int rows, int cols)
{
	if (rows < rowCount || cols < colCount)
	{
		throw std::exception("Can't reduce matrix size");
	}
	rowCount = rows;
	colCount = cols;
}

template<class T>
LLSparseMatrix<T>::~LLSparseMatrix()
{
	if (!firstNode)
	{
		return;
	}
	if (!firstNode->nextNode)
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
		throw std::exception("Element indices are out of bounds");
	}
	for (auto node = firstNode; node != nullptr; node = node->nextNode)
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
		throw std::exception("Element indices are out of bounds");
	}
	if (val == T())
	{
		return;
	}
	++nonZeroElementsCount;
	if (firstNode == nullptr)
	{
		firstNode = new MatrixNode(row, col, val);
		return;
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
		MatrixNode *newNode = new MatrixNode(row, col, val);
		prevNode->nextNode = newNode;
	}
}

template<class T>
bool LLSparseMatrix<T>::RemoveElement(int row, int col)
{
	if (!InBoundaries(row, col))
	{
		throw std::exception("Element indices are out of bounds");
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
			return true;
		}
		prevNode = node;
	}
	return false;
}


template<class T>
void LLSparseMatrix<T>::Print(std::ostream &os) const
{
	auto node = firstNode;
	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < colCount; j++)
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
void LLSparseMatrix<T>::Transponse()
{
	for (MatrixNode *node = firstNode; node != nullptr; node = node->nextNode)
	{
		std::swap(node->row, node->col);
	}
	std::swap(rowCount, colCount);
	SortByPosition(&firstNode);
}


template<class T>
LLSparseMatrix<T> *LLSparseMatrix<T>::Multiply(LLSparseMatrix<T> *other)
{
	if (this->colCount != other->rowCount)
	{
		throw std::exception("Invalid argument: impossible to multiply incompatible matrices");
	}
	auto *result = new LLSparseMatrix(this->rowCount, other->colCount);
	other->Transponse();
	auto *thisItr = this->firstNode;
	auto *otherItr = other->firstNode;
	std::map< std::pair<int, int>, T> idxValMap;
	auto *currentRowStart = thisItr;
	bool isLastRow = false;
	// Resulting row is a ROW of FIRST matrix
	// Resulting col is a ROW of SECOND matrix
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
			//if (isLastRow)
			//{
			//	// We done
			//	break;
			//}

			// We done with current row, switch to another

			// WE CAN REACH THE END OF OTHER MAT BEFORE WE REACH THE END OF CURRENT ROW
			while (!(currentRowStart->row < thisItr->row))
			{
				if (thisItr->nextNode == nullptr)
				{
					// We're on the last row, so we're done
					isLastRow = true;
					break;
				}
				thisItr = thisItr->nextNode;
			}
			if (isLastRow)
			{
				// We done
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

		std::cout << thisItr->value << " <---> ";
		std::cout << otherItr->value << std::endl;

		if (thisItr->col == otherItr->col)
		{
			// Need to multiply
			int i = thisItr->row;
			int j = otherItr->row;
			idxValMap[std::pair<int, int>(i, j)] += thisItr->value * otherItr->value;


			// MOVE BOTH ITRS IF THEY BOTH WILL BE ON THE SAME LINE
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
	other->Transponse();
	for (auto item : idxValMap)
	{
		auto [indices, value] = item;
		auto [i, j] = indices;
		std::cout << "(" << i << ", " << j << ")" << " = " << value << std::endl;

		// Create result matrix
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
	*head = MergeSortedLists(split1, split2);
}

template<class T>
void LLSparseMatrix<T>::SplitList(MatrixNode *head, MatrixNode **first, MatrixNode **second)
{
	// Floyd's tortoise algorithm finding middle of linked list
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
typename LLSparseMatrix<T>::MatrixNode *LLSparseMatrix<T>::MergeSortedLists(MatrixNode *list1, MatrixNode *list2)
{
	MatrixNode *newHead = nullptr;
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
		newHead->nextNode = MergeSortedLists(list1->nextNode, list2);
	}
	else
	{
		newHead = list2;
		newHead->nextNode = MergeSortedLists(list1, list2->nextNode);
	}
	return newHead;
}

template<class T>
bool LLSparseMatrix<T>::InBoundaries(int row, int col) const
{
	return (row < rowCount && row >= 0) && (col < colCount && col >= 0);
}

template<class T>
int LLSparseMatrix<T>::GetPosition(int row, int col)
{
	return colCount * row + col;
}

template<class T>
std::ostream &operator<<(std::ostream &os, LLSparseMatrix<T> &sm)
{
	sm.Print(os);
	return os;
}