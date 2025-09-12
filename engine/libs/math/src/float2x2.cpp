#include "float2x2.h"
#include "float2.h"
#include "mathFunctions.h"
#include <stdexcept>
#include <sstream>



namespace emberMath
{
	// Constructors:
	Float2x2::Float2x2()
	{
		for (uint32_t i = 0; i < 4; i++)
			data[i] = 0.0f;
	}
	Float2x2::Float2x2(float value)
	{
		for (uint32_t i = 0; i < 4; i++)
			data[i] = value;
	}
	Float2x2::Float2x2(const float* const array)
	{
		for (uint32_t i = 0; i < 4; i++)
			data[i] = array[i];
	}
	Float2x2::Float2x2
	(float xx, float xy,	// column 0
		float yx, float yy)	// column 1
	{
		data[0] = xx; data[2] = yx;
		data[1] = xy; data[3] = yy;
	}
	Float2x2::Float2x2(const Float2x2& other)
	{
		for (uint32_t i = 0; i < 4; i++)
			data[i] = other.data[i];
	}



	// Static constructors:
	Float2x2 Float2x2::Rows(const Float2& row0, const Float2& row1)
	{
		return Float2x2
		(row0.x, row1.x,
			row0.y, row1.y);
	}
	Float2x2 Float2x2::Rows
	(float row0x, float row0y,
		float row1x, float row1y)
	{
		return Float2x2
		(row0x, row1x,
			row0y, row1y);
	}
	Float2x2 Float2x2::Columns(const Float2& column0, const Float2& column1)
	{
		return Float2x2
		(column0.x, column0.y,
			column1.x, column1.y);
	}
	Float2x2 Float2x2::Columns
	(float column0x, float column0y,
		float column1x, float column1y)
	{
		return Float2x2
		(column0x, column0y,
			column1x, column1y);
	}



	// Math operations:
	Float2x2 Float2x2::Transpose() const
	{
		return Float2x2
		(data[0], data[2],
		 data[1], data[3]);
	}
	float Float2x2::Determinant() const
	{
		return data[0] * data[3] - data[1] * data[2];
	}
	Float2x2 Float2x2::Inverse() const
	{
		float det = Determinant();
		return Inverse(det);
	}
	Float2x2 Float2x2::Inverse(float det) const
	{
		if (det == 0.0f)
			return Float2x2::zero;
		float invDet = 1.0f / det;
		return Float2x2::Columns
		(
			invDet * data[3], -invDet * data[1],
			-invDet * data[2], invDet * data[0]
		);
	}
	bool Float2x2::IsEpsilonZero() const
	{
		return IsEpsilonEqual(Float2x2::zero);
	}



	// Static math operations:
	Float2x2 Float2x2::Rotate(float radians)
	{
		float c = math::Cos(radians);
		float s = math::Sin(radians);
		return Float2x2::Rows
		(c, -s,
			s, c);
	}



	// Access:
	float& Float2x2::operator[](const Index2& index)
	{
		if (index.i >= 0 && index.i < 2 && index.j >= 0 && index.j < 2)
			return data[index.i + 2 * index.j];
		throw std::out_of_range("Float2x2 index out of range.");
	}
	float Float2x2::operator[](const Index2& index) const
	{
		if (index.i >= 0 && index.i < 2 && index.j >= 0 && index.j < 2)
			return data[index.i + 2 * index.j];
		throw std::out_of_range("Float2x2 index out of range.");
	}
	Float2 Float2x2::GetRow(int index) const
	{
		if (index >= 0 && index < 2)
			return Float2(data[index], data[index + 2]);
		throw std::out_of_range("Float2x2 row index out of range.");
	}
	Float2 Float2x2::GetColumn(int index) const
	{
		if (index >= 0 && index < 2)
			return Float2(data[2 * index], data[2 * index + 1]);
		throw std::out_of_range("Float2x2 column index out of range.");
	}



	// Assignment:
	Float2x2& Float2x2::operator=(const Float2x2& other)
	{
		if (this != &other)
		{
			for (uint32_t i = 0; i < 4; i++)
				data[i] = other.data[i];
		}
		return *this;
	}
	Float2x2& Float2x2::operator=(Float2x2&& other) noexcept
	{
		if (this != &other)
		{
			for (uint32_t i = 0; i < 4; i++)
				data[i] = other.data[i];
		}
		return *this;
	}



	// Addition:
	Float2x2 Float2x2::operator+(const Float2x2& other) const
	{
		Float2x2 result;
		for (uint32_t i = 0; i < 4; i++)
			result.data[i] = data[i] + other.data[i];
		return result;
	}
	Float2x2& Float2x2::operator+=(const Float2x2& other)
	{
		for (uint32_t i = 0; i < 4; i++)
			data[i] += other.data[i];
		return *this;
	}



	// Substraction:
	Float2x2 Float2x2::operator-(const Float2x2& other) const
	{
		Float2x2 result;
		for (uint32_t i = 0; i < 4; i++)
			result.data[i] = data[i] - other.data[i];
		return result;
	}
	Float2x2& Float2x2::operator-=(const Float2x2& other)
	{
		for (uint32_t i = 0; i < 4; i++)
			data[i] -= other.data[i];
		return *this;
	}
	Float2x2 Float2x2::operator-() const
	{
		return Float2x2
		(-data[0], -data[1],
			-data[2], -data[3]);
	}



	// Multiplication:
	Float2x2 Float2x2::operator*(const Float2x2& other) const
	{
		Float2x2 result;
		for (uint32_t i = 0; i < 2; i++)
			for (uint32_t j = 0; j < 2; j++)
				for (uint32_t k = 0; k < 2; k++)
					result[{i, j}] += (*this)[{i, k}] * other[{k, j}];
		return result;
	}
	Float2x2& Float2x2::operator*=(const Float2x2& other)
	{
		Float2x2 result = (*this) * other;
		*this = result;
		return *this;
	}
	Float2x2& Float2x2::operator*=(float scalar)
	{
		for (uint32_t i = 0; i < 4; i++)
			data[i] *= scalar;
		return *this;
	}



	// Division:
	Float2x2 Float2x2::operator/(float scalar) const
	{
		Float2x2 result;
		for (uint32_t i = 0; i < 4; i++)
			result.data[i] = data[i] / scalar;
		return result;
	}
	Float2x2& Float2x2::operator/=(float scalar)
	{
		for (uint32_t i = 0; i < 4; i++)
			data[i] /= scalar;
		return *this;
	}



	// Comparison:
	bool Float2x2::IsEpsilonEqual(const Float2x2& other) const
	{
		for (uint32_t i = 0; i < 4; i++)
			if (math::Abs(data[i] - other.data[i]) > math::epsilon)
				return false;
		return true;
	}
	bool Float2x2::operator==(const Float2x2& other) const
	{
		for (uint32_t i = 0; i < 4; i++)
			if (data[i] != other.data[i])
				return false;
		return true;
	}
	bool Float2x2::operator!=(const Float2x2& other) const
	{
		return !(*this == other);
	}



	// Friend functions:
	Float2x2 operator*(const Float2x2& a, float b)
	{
		Float2x2 result;
		for (uint32_t i = 0; i < 4; i++)
			result.data[i] = a.data[i] * b;
		return result;
	}
	Float2x2 operator*(float a, const Float2x2& b)
	{
		Float2x2 result;
		for (uint32_t i = 0; i < 4; i++)
			result.data[i] = a * b.data[i];
		return result;
	}
	Float2 operator*(const Float2x2& a, const Float2& b)
	{
		return Float2
		(a.data[0] * b.x + a.data[2] * b.y,
			a.data[1] * b.x + a.data[3] * b.y);
	}
	Float2 operator*(const Float2& a, const Float2x2& b)
	{
		return Float2
		(a.x * b.data[0] + a.y * b.data[1],
			a.x * b.data[2] + a.y * b.data[3]);
	}



	// Logging:
	std::string Float2x2::ToString() const
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1];
		oss << " | " << data[2] << ", " << data[3] << ")";
		return oss.str();
	}
	std::string Float2x2::ToStringMatrixForm() const
	{
		std::ostringstream oss;
		oss << "\n";
		oss << "(" << data[0] << ", " << data[2] << ")\n";
		oss << "(" << data[1] << ", " << data[3] << ")";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Float2x2& value)
	{
		os << value.ToString();
		return os;
	}



	// Static members:
	Float2x2 Float2x2::zero = Float2x2(0.0f);
	Float2x2 Float2x2::identity = Float2x2
	(1.0f, 0.0f,
		0.0f, 1.0f);
	Float2x2 Float2x2::maxValue = Float2x2(math::maxValue);
	Float2x2 Float2x2::minValue = Float2x2(math::minValue);
}