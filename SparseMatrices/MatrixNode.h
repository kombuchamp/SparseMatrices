#pragma once

template<typename T>
struct MatrixNode
{
	MatrixNode(const size_t row, const size_t col, T const &val)
		: Row(row), Col(col), Value(val)
	{
	}
	size_t Row;
	size_t Col;
	T Value;
};