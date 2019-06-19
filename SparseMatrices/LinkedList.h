#pragma once
#include <stdexcept>
#include <iostream>
#include <functional>

template <typename T>
struct LinkedListNode
{
	LinkedListNode<T> *Next;
	LinkedListNode<T> *Prev;
	T Value;
};

template <typename T>
class LinkedList
{
public:
	~LinkedList()
	{
		Clear();
	}
	size_t Count() const
	{
		return _count;
	}

	LinkedListNode<T> *First()
	{
		return _first;
	}

	LinkedListNode<T> *Last()
	{
		return _last;
	}

	void AddFirst(T const &value)
	{
		auto *newNode = new LinkedListNode<T>{ nullptr, nullptr, value };
		if (_first == nullptr)
		{
			_first = newNode;
			_last = newNode;
		}
		else
		{
			newNode->Next = _first;
			_first->Prev = newNode;
			_first = newNode;
		}
		++_count;
	}

	void AddLast(T const &value)
	{
		auto *newNode = new LinkedListNode<T>{ nullptr, nullptr, value };
		if (_last == nullptr)
		{
			_first = newNode;
			_last = newNode;
		}
		else
		{
			_last->Next = newNode;
			newNode->Prev = _last;
			_last = newNode;
		}
		++_count;
	}

	void AddAfter(LinkedListNode<T> *node, T const &value)
	{
		if (!ValidateNode(node))
		{
			throw std::invalid_argument("Node is not from this list");
		}
		auto *newNode = new LinkedListNode<T>();
		newNode->Value = value;
		newNode->Next = node->Next;
		newNode->Prev = node;
		node->Next->Prev = newNode;
		node->Next = newNode;
		++_count;
	}

	void Clear()
	{
		if (_first != nullptr)
		{
			ClearRecursive(_first);
		}
		_first = nullptr;
		_last = nullptr;
		_count = 0;
	}

	bool Contains(T const &value)
	{
		return Find(value) != nullptr;
	}

	LinkedListNode<T> *Find(T const &value)
	{
		if (_first == nullptr)
		{
			return nullptr;
		}
		for (auto current = _first; current != nullptr; current = current->Next)
		{
			if (current->Value == value)
			{
				return current;
			}
		}
		return nullptr;
	}

	bool Remove(T const &value)
	{
		if (_first == nullptr)
		{
			return false;
		}

		for (auto current = _first; current != nullptr; current = current->Next)
		{
			if (current->Value == value)
			{
				if (current == _first)
				{
					auto old = _first;
					_first = _first->Next;
					_first->Prev = nullptr;
					delete old;
				}
				else
				{
					auto old = current;
					current->Prev->Next = current->Next;
					current->Next->Prev = current->Prev;
					delete old;
				}
				--_count;
				return true;
			}
		}
		return false;
	}


	void Sort(std::function<bool(T a, T b)> Cmp)
	{
		MergeSort(&_first, Cmp);
	}

private:
	bool ValidateNode(LinkedListNode<T> const *node)
	{
		if (node == nullptr)
		{
			return false;
		}
		for (auto current = _first; current != nullptr; current = current->Next)
		{
			if (current == node)
			{
				return true;
			}
		}
		return false;
	}

	void ClearRecursive(LinkedListNode<T> *node)
	{
		if (node->Next == nullptr)
		{
			delete node;
			return;
		}
		ClearRecursive(node->Next);
		delete node;
	}
	
	void MergeSort(LinkedListNode<T> **head, std::function<bool(T a, T b)> Cmp)
	{
		LinkedListNode<T> *currentHead = *head;
		LinkedListNode<T> *split1, *split2;
		if (currentHead == nullptr || currentHead->Next == nullptr)
		{
			return;
		}
		SplitList(currentHead, &split1, &split2);
		MergeSort(&split1, Cmp);
		MergeSort(&split2, Cmp);
		*head = MergeLists(split1, split2, Cmp);
	}

	void SplitList(LinkedListNode<T> *head, LinkedListNode<T> **first, LinkedListNode<T> **second)
	{
		// Floyd's tortoise algorithm of finding middle of linked list
		LinkedListNode<T> *slow = head;
		LinkedListNode<T> *fast = head->Next;

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
		(*second)->Prev = nullptr;
	}


	LinkedListNode<T> *MergeLists(LinkedListNode<T> *list1, LinkedListNode<T> *list2, std::function<bool(T a, T b)> Cmp)
	{
		LinkedListNode<T> *newHead;
		if (list1 == nullptr)
		{
			return list2;
		}
		if (list2 == nullptr)
		{
			return list1;
		}
		if (Cmp(list1->Value, list2->Value))
		{
			newHead = list1;
			newHead->Next = MergeLists(list1->Next, list2, Cmp);
			newHead->Next->Prev = newHead;
		}
		else
		{
			newHead = list2;
			newHead->Next = MergeLists(list1, list2->Next, Cmp);
			newHead->Next->Prev = newHead;
		}
		return newHead;
	}

	size_t _count = 0;
	LinkedListNode<T> *_first = nullptr;
	LinkedListNode<T> *_last = nullptr;
};