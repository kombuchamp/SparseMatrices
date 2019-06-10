#include "pch.h"
#include "CppUnitTest.h"
#include "../SparseMatrices/ISparseMatrix.h"
#include "../SparseMatrices/LLSparseMatrix.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SparseMatrices_Tests
{
	TEST_CLASS(LLSparseMatrix_Tests)
	{
	public:
		TEST_METHOD(ShouldResizeCorrectly)
		{
			LLSparseMatrix<> mat;

			const size_t newRows = 4;
			const size_t newCols = 5;

			mat.Resize(newRows, newCols);
			
			Assert::AreEqual(newRows, mat.GetRowCount());
			Assert::AreEqual(newCols, mat.GetColCount());
		}

		TEST_METHOD(ShouldSetElements)
		{
			LLSparseMatrix<> mat(4, 4);

			mat.SetElement(0, 0, 1.);
			mat.SetElement(1, 1, 1.);
			mat.SetElement(1, 2, 1.);

			Assert::AreEqual(1., mat.ElementAt(0, 0));
			Assert::AreEqual(1., mat.ElementAt(1, 1));
			Assert::AreEqual(1., mat.ElementAt(1, 2));
			Assert::AreEqual(0., mat.ElementAt(3, 3));
		}

		TEST_METHOD(ShouldRemoveElements)
		{
			LLSparseMatrix<> mat(4, 4);

			mat.SetElement(0, 0, 1.);
			mat.RemoveElement(0, 0);
			mat.RemoveElement(1, 1);

			Assert::AreEqual(0., mat.ElementAt(0, 0));
			Assert::AreEqual(0., mat.ElementAt(1, 1));
		}

		TEST_METHOD(ThrowIfSettingElementOutOfBounds)
		{
			LLSparseMatrix<> mat(1, 1);

			Assert::ExpectException<std::exception>([&]()
				{
					mat.SetElement(100, 100, 1);
				});
		}

		TEST_METHOD(ThrowIfResizeWithDataLoss)
		{
			LLSparseMatrix<> mat(100, 100);

			Assert::ExpectException<std::exception>([&]()
				{
					mat.Resize(1, 1);
				});
		}

		TEST_METHOD(ThrowIfGettingElementOutOfBounds)
		{
			LLSparseMatrix<> mat;

			Assert::ExpectException<std::exception>([&]()
				{
					mat.ElementAt(100, 100);
				});
		}

		TEST_METHOD(ThrowIfRemovingElementOutOfBounds)
		{
			LLSparseMatrix<> mat;

			Assert::ExpectException<std::exception>([&]()
				{
					mat.RemoveElement(100, 100);
				});
		}

		TEST_METHOD(ShouldPrintOutMatrix)
		{
			LLSparseMatrix<> mat(2, 2);
			mat.SetElement(0, 0, 1.);
			mat.SetElement(0, 1, 1.);
			mat.SetElement(1, 0, 2.);
			mat.SetElement(1, 1, 2.);
			
			std::stringstream buf;
			double tmp;
			buf << mat;

			buf >> tmp;
			Assert::AreEqual(1., tmp);
			buf >> tmp;
			Assert::AreEqual(1., tmp);
			buf >> tmp;
			Assert::AreEqual(2., tmp);
			buf >> tmp;
			Assert::AreEqual(2., tmp);
		}

		TEST_METHOD(ShouldTransposeMatrix)
		{
			LLSparseMatrix<> mat(2, 2);
			mat.SetElement(0, 0, 1.);
			mat.SetElement(0, 1, 1.);
			mat.SetElement(1, 0, 2.);
			mat.SetElement(1, 1, 2.);

			mat.Transpose();

			Assert::AreEqual(1., mat.ElementAt(0, 0));
			Assert::AreEqual(2., mat.ElementAt(0, 1));
			Assert::AreEqual(1., mat.ElementAt(1, 0));
			Assert::AreEqual(2., mat.ElementAt(1, 1));
		}

		TEST_METHOD(ShouldMultiplyDenseMatrices)
		{
			LLSparseMatrix<int> mat0(2, 3);
			LLSparseMatrix<int> mat1(3, 2);

			mat0.SetElement(0, 0, 1);
			mat0.SetElement(0, 1, 2);
			mat0.SetElement(0, 2, 3);
			mat0.SetElement(1, 0, 4);
			mat0.SetElement(1, 1, 5);
			mat0.SetElement(1, 2, 6);

			mat1.SetElement(0, 0, 7);
			mat1.SetElement(0, 1, 8);
			mat1.SetElement(1, 0, 9);
			mat1.SetElement(1, 1, 10);
			mat1.SetElement(2, 0, 11);
			mat1.SetElement(2, 1, 12);

			auto resultMat = mat0.Multiply(mat1);

			Assert::AreEqual(58, resultMat->ElementAt(0, 0));
			Assert::AreEqual(64, resultMat->ElementAt(0, 1));
			Assert::AreEqual(139, resultMat->ElementAt(1, 0));
			Assert::AreEqual(154, resultMat->ElementAt(1, 1));
		}

		TEST_METHOD(ShouldMultiplySparseMatrices_0)
		{
			LLSparseMatrix<int> mat0(2, 3);
			LLSparseMatrix<int> mat1(3, 2);

			mat0.SetElement(0, 0, 0);
			mat0.SetElement(0, 1, 1);
			mat0.SetElement(0, 2, 0);
			mat0.SetElement(1, 0, 2);
			mat0.SetElement(1, 1, 0);
			mat0.SetElement(1, 2, 3);

			mat1.SetElement(0, 0, 4);
			mat1.SetElement(0, 1, 0);
			mat1.SetElement(1, 0, 0);
			mat1.SetElement(1, 1, 6);
			mat1.SetElement(2, 0, 5);
			mat1.SetElement(2, 1, 0);

			auto resultMat = mat0.Multiply(mat1);

			Assert::AreEqual(0, resultMat->ElementAt(0, 0));
			Assert::AreEqual(6, resultMat->ElementAt(0, 1));
			Assert::AreEqual(23, resultMat->ElementAt(1, 0));
			Assert::AreEqual(0, resultMat->ElementAt(1, 1));
		}

		TEST_METHOD(ShouldMultiplySparseMatrices_1)
		{
			LLSparseMatrix<int> mat0(2, 3);
			LLSparseMatrix<int> mat1(3, 2);

			mat0.SetElement(0, 0, 1);
			mat0.SetElement(0, 1, 0);
			mat0.SetElement(0, 2, 2);
			mat0.SetElement(1, 0, 0);
			mat0.SetElement(1, 1, 3);
			mat0.SetElement(1, 2, 0);

			mat1.SetElement(0, 0, 0);
			mat1.SetElement(0, 1, 4);
			mat1.SetElement(1, 0, 5);
			mat1.SetElement(1, 1, 0);
			mat1.SetElement(2, 0, 0);
			mat1.SetElement(2, 1, 6);

			auto resultMat = mat0.Multiply(mat1);

			Assert::AreEqual(0, resultMat->ElementAt(0, 0));
			Assert::AreEqual(16, resultMat->ElementAt(0, 1));
			Assert::AreEqual(15, resultMat->ElementAt(1, 0));
			Assert::AreEqual(0, resultMat->ElementAt(1, 1));
		}

		TEST_METHOD(ShouldMultiplySparseMatrices_2)
		{
			LLSparseMatrix<int> mat0(3, 3);
			LLSparseMatrix<int> mat1(3, 3);

			mat0.SetElement(0, 0, 1);
			mat0.SetElement(0, 1, 2);
			mat0.SetElement(0, 2, 3);
			mat0.SetElement(1, 0, 0);
			mat0.SetElement(1, 1, 0);
			mat0.SetElement(1, 2, 0);
			mat0.SetElement(2, 0, 4);
			mat0.SetElement(2, 1, 5);
			mat0.SetElement(2, 2, 6);

			mat1.SetElement(0, 0, 0);
			mat1.SetElement(0, 1, 0);
			mat1.SetElement(0, 2, 0);
			mat1.SetElement(1, 0, 7);
			mat1.SetElement(1, 1, 8);
			mat1.SetElement(1, 2, 9);
			mat1.SetElement(2, 0, 0);
			mat1.SetElement(2, 1, 0);
			mat1.SetElement(2, 2, 0);

			auto resultMat = mat0.Multiply(mat1);

			Assert::AreEqual(14, resultMat->ElementAt(0, 0));
			Assert::AreEqual(16, resultMat->ElementAt(0, 1));
			Assert::AreEqual(18, resultMat->ElementAt(0, 2));
			Assert::AreEqual(0, resultMat->ElementAt(1, 0));
			Assert::AreEqual(0, resultMat->ElementAt(1, 1));
			Assert::AreEqual(0, resultMat->ElementAt(1, 2));
			Assert::AreEqual(35, resultMat->ElementAt(2, 0));
			Assert::AreEqual(40, resultMat->ElementAt(2, 1));
			Assert::AreEqual(45, resultMat->ElementAt(2, 2));
		}

		TEST_METHOD(ShouldMultiplySparseMatrices_3)
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

			auto resultMat = mat0.Multiply(mat1);

			Assert::AreEqual(0, resultMat->ElementAt(0, 0));
			Assert::AreEqual(43, resultMat->ElementAt(0, 1));
			Assert::AreEqual(0, resultMat->ElementAt(0, 2));
			Assert::AreEqual(0, resultMat->ElementAt(1, 0));
			Assert::AreEqual(59, resultMat->ElementAt(1, 1));
			Assert::AreEqual(0, resultMat->ElementAt(1, 2));
			Assert::AreEqual(0, resultMat->ElementAt(2, 0));
			Assert::AreEqual(75, resultMat->ElementAt(2, 1));
			Assert::AreEqual(0, resultMat->ElementAt(2, 2));
		}

		TEST_METHOD(ShouldMultiplySparseMatrices_4)
		{
			LLSparseMatrix<int> mat0(3, 3);
			LLSparseMatrix<int> mat1(3, 3);

			mat0.SetElement(0, 0, 0);
			mat0.SetElement(0, 1, 0);
			mat0.SetElement(0, 2, 0);
			mat0.SetElement(1, 0, 1);
			mat0.SetElement(1, 1, 2);
			mat0.SetElement(1, 2, 3);
			mat0.SetElement(2, 0, 0);
			mat0.SetElement(2, 1, 0);
			mat0.SetElement(2, 2, 0);

			mat1.SetElement(0, 0, 4);
			mat1.SetElement(0, 1, 5);
			mat1.SetElement(0, 2, 6);
			mat1.SetElement(1, 0, 0);
			mat1.SetElement(1, 1, 0);
			mat1.SetElement(1, 2, 0);
			mat1.SetElement(2, 0, 7);
			mat1.SetElement(2, 1, 8);
			mat1.SetElement(2, 2, 9);

			auto resultMat = mat0.Multiply(mat1);

			Assert::AreEqual(0, resultMat->ElementAt(0, 0));
			Assert::AreEqual(0, resultMat->ElementAt(0, 1));
			Assert::AreEqual(0, resultMat->ElementAt(0, 2));
			Assert::AreEqual(25, resultMat->ElementAt(1, 0));
			Assert::AreEqual(29, resultMat->ElementAt(1, 1));
			Assert::AreEqual(33, resultMat->ElementAt(1, 2));
			Assert::AreEqual(0, resultMat->ElementAt(2, 0));
			Assert::AreEqual(0, resultMat->ElementAt(2, 1));
			Assert::AreEqual(0, resultMat->ElementAt(2, 2));
		}
	};
}