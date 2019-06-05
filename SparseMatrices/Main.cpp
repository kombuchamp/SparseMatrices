#include "LLSparseMatrix.h"
#include <iostream>
#include <memory>

using namespace std;

#define OUTPUT(X) std::cout << (X) << std::endl

int main()
{
	LLSparseMatrix<int> mat0(2, 2);
	LLSparseMatrix<int> mat1(2, 2);

	mat0.SetElement(0, 0, 0);
	mat0.SetElement(0, 1, 0);
	mat0.SetElement(1, 0, 1);
	mat0.SetElement(1, 1, 1);

	mat1.SetElement(0, 0, 1);
	mat1.SetElement(0, 1, 1);
	mat1.SetElement(1, 0, 0);
	mat1.SetElement(1, 1, 0);

	OUTPUT("***MATRICES***");
	std::cout << mat0 << "\n" << std::endl;
	std::cout << mat1 << "\n" << std::endl;
	OUTPUT("*STARTING PROCESS...*");
	std::unique_ptr<LLSparseMatrix<int>> mat2(mat0 * mat1);
	std::cout << "DONE: " << std::endl;
	std::cout << *mat2;

	return 0;
}