#include "LLSparseMatrix.h"
#include <iostream>
#include <memory>

using namespace std;

#define OUTPUT(X) std::cout << (X) << std::endl

int main()
{
	LLSparseMatrix<int> mat0(3, 3);
	LLSparseMatrix<int> mat1(3, 3);

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

	OUTPUT("***MATRICES***");
	std::cout << mat0 << "\n" << std::endl;
	std::cout << mat1 << "\n" << std::endl;
	OUTPUT("*STARTING PROCESS...*");
	auto mat2 = mat0 * mat1;
	std::cout << "DONE: " << std::endl;
	std::cout << *mat2;

	return 0;
}