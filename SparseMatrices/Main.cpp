#include "LLSparseMatrix.h"
#include <iostream>
#include <memory>
using std::cout;
using std::endl;
#include "LinkedList.h"

int main()
{
	LinkedList<int> l;
	l.AddFirst(12);
	auto a = l.Find(12);

	l.AddBefore(a, 2);
	l.AddAfter(a, 90);

	for (auto item : l)
	{
		cout << item << " ";
	}

	LLSparseMatrix<int> mat0(3, 3);
	LLSparseMatrix<int> mat1(3, 3);
/*
	mat0.SetElement(0, 0, 1);
	mat0.SetElement(0, 1, 0);
	mat0.SetElement(0, 2, 4);
	mat0.SetElement(1, 0, 2);
	mat0.SetElement(1, 1, 0);
	mat0.SetElement(1, 2, 5);
	mat0.SetElement(2, 0, 3);
	mat0.SetElement(2, 1, 0);
	mat0.SetElement(2, 2, 6);

	mat1.SetElement(0, 0, 0);
	mat1.SetElement(0, 1, 7);
	mat1.SetElement(0, 2, 0);
	mat1.SetElement(1, 0, 0);
	mat1.SetElement(1, 1, 8);
	mat1.SetElement(1, 2, 0);
	mat1.SetElement(2, 0, 0);
	mat1.SetElement(2, 1, 9);
	mat1.SetElement(2, 2, 0);

	cout << "***MATRICES***" << endl;
	cout << mat0 << endl;
	cout << mat1 << endl;
	cout << "*STARTING PROCESS...*" << endl;

	std::unique_ptr<LLSparseMatrix<int>> mat2(mat0 * mat1);

	cout << "DONE: " << endl;
	cout << *mat2 << endl;

	return 0;*/
}