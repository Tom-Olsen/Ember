#include "float2x3.h"
#include "float2.h"
#include "float3.h"
#include "float2x2.h"
#include "float3x2.h"
#include "float3x3.h"
#include "mathFunctions.h"
#include <stdexcept>
#include <sstream>



namespace emberMath
{
	// Constructors:
	Float2x3::Float2x3
	(float xx, float xy,	// column 0
		float yx, float yy,	// column 1
		float zx, float zy)	// column 2
	{
		data[0] = xx; data[2] = yx; data[4] = zx;
		data[1] = xy; data[3] = yy; data[5] = zy;
	}
	Float2x3::Float2x3()
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] = 0.0f;
	}
	Float2x3::Float2x3(float value)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] = value;
	}
	Float2x3::Float2x3(const float* const array)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] = array[i];
	}
	Float2x3::Float2x3(const Float2x3& other)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] = other[i];
	}



	// Static constructors:
	Float2x3 Float2x3::Rows(const Float3& row0, const Float3& row1)
	{
		return Float2x3
		(row0.x, row1.x,
			row0.y, row1.y,
			row0.z, row1.z);
	}
	Float2x3 Float2x3::Rows
	(float row0x, float row0y, float row0z,
		float row1x, float row1y, float row1z)
	{
		return Float2x3
		(row0x, row1x,
			row0y, row1y,
			row0z, row1z);
	}
	Float2x3 Float2x3::Columns(const Float2& column0, const Float2& column1, const Float2& column2)
	{
		return Float2x3
		(column0.x, column0.y,
			column1.x, column1.y,
			column2.x, column2.y);
	}
	Float2x3 Float2x3::Columns
	(float column0x, float column0y,
		float column1x, float column1y,
		float column2x, float column2y)
	{
		return Float2x3
		(column0x, column0y,
			column1x, column1y,
			column2x, column2y);
	}



	// Math operations:
	Float3x2 Float2x3::Transpose() const
	{
		return Float3x2::Rows
		(data[0], data[1],
			data[2], data[3],
			data[4], data[5]);
	}
	Float3x2 Float2x3::RightInverse() const
	{
		Float3x2 aT = this->Transpose();
		return aT * ((*this) * aT).Inverse();
	}
	bool Float2x3::IsEpsilonZero() const
	{
		return IsEpsilonEqual(Float2x3::zero);
	}



	// Access:
	float& Float2x3::operator[](int index)
	{
		if (index >= 0 && index < 6)
			return data[index];
		throw std::out_of_range("Float2x3 index out of range.");
	}
	float Float2x3::operator[](int index) const
	{
		if (index >= 0 && index < 6)
			return data[index];
		throw std::out_of_range("Float2x3 index out of range.");
	}
	float& Float2x3::operator[](const Index2& index)
	{
		if (index.i >= 0 && index.i < 2 && index.j >= 0 && index.j < 3)
			return data[index.i + 2 * index.j];
		throw std::out_of_range("Float2x3 index out of range.");
	}
	float Float2x3::operator[](const Index2& index) const
	{
		if (index.i >= 0 && index.i < 2 && index.j >= 0 && index.j < 3)
			return data[index.i + 2 * index.j];
		throw std::out_of_range("Float2x3 index out of range.");
	}
	Float3 Float2x3::GetRow(int index) const
	{
		if (index >= 0 && index < 2)
			return Float3(data[index], data[index + 2], data[index + 4]);
		throw std::out_of_range("Float2x3 row index out of range.");
	}
	Float2 Float2x3::GetColumn(int index) const
	{
		if (index >= 0 && index < 3)
			return Float2(data[2 * index], data[2 * index + 1]);
		throw std::out_of_range("Float2x3 column index out of range.");
	}



	// Assignment:
	Float2x3& Float2x3::operator=(const Float2x3& other)
	{
		if (this != &other)
		{
			for (uint32_t i = 0; i < 6; i++)
				data[i] = other[i];
		}
		return *this;
	}
	Float2x3& Float2x3::operator=(Float2x3&& other) noexcept
	{
		if (this != &other)
		{
			for (uint32_t i = 0; i < 6; i++)
				data[i] = other[i];
		}
		return *this;
	}



	// Addition:
	Float2x3 Float2x3::operator+(const Float2x3& other) const
	{
		Float2x3 result;
		for (uint32_t i = 0; i < 6; i++)
			result[i] = data[i] + other[i];
		return result;
	}
	Float2x3& Float2x3::operator+=(const Float2x3& other)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] += other[i];
		return *this;
	}



	// Substraction:
	Float2x3 Float2x3::operator-(const Float2x3& other) const
	{
		Float2x3 result;
		for (uint32_t i = 0; i < 6; i++)
			result[i] = data[i] - other[i];
		return result;
	}
	Float2x3& Float2x3::operator-=(const Float2x3& other)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] -= other[i];
		return *this;
	}
	Float2x3 Float2x3::operator-() const
	{
		return Float2x3
		(-data[0], -data[1],
			-data[2], -data[3],
			-data[4], -data[5]);
	}



	// Multiplication:
	Float2x3& Float2x3::operator*=(float scalar)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] *= scalar;
		return *this;
	}



	// Division:
	Float2x3 Float2x3::operator/(float scalar) const
	{
		Float2x3 result;
		for (uint32_t i = 0; i < 6; i++)
			result[i] = data[i] / scalar;
		return result;
	}
	Float2x3& Float2x3::operator/=(float scalar)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] /= scalar;
		return *this;
	}



	// Comparison:
	bool Float2x3::IsEpsilonEqual(const Float2x3& other) const
	{
		for (uint32_t i = 0; i < 6; i++)
			if (math::Abs(data[i] - other[i]) > math::epsilon)
				return false;
		return true;
	}
	bool Float2x3::operator==(const Float2x3& other) const
	{
		for (uint32_t i = 0; i < 6; i++)
			if (data[i] != other[i])
				return false;
		return true;
	}
	bool Float2x3::operator!=(const Float2x3& other) const
	{
		return !(*this == other);
	}



	// Friend functions:
	Float2x3 operator*(const Float2x3& a, float b)
	{
		Float2x3 result;
		for (uint32_t i = 0; i < 6; i++)
			result[i] = a[i] * b;
		return result;
	}
	Float2x3 operator*(float a, const Float2x3& b)
	{
		Float2x3 result;
		for (uint32_t i = 0; i < 6; i++)
			result[i] = a * b[i];
		return result;
	}
	Float2 operator*(const Float2x3& a, const Float3& b)
	{
		return Float2
		(a[0] * b.x + a[2] * b.y + a[4] * b.z,
			a[1] * b.x + a[3] * b.y + a[5] * b.z);
	}
	Float3 operator*(const Float2& a, const Float2x3& b)
	{
		return Float3
		(a.x * b[0] + a.y * b[1],
			a.x * b[2] + a.y * b[3],
			a.x * b[4] + a.y * b[5]);
	}
	Float2x2 operator*(const Float2x3& a, const Float3x2& b)
	{
		Float2x2 result;
		for (uint32_t i = 0; i < 2; i++)
			for (uint32_t j = 0; j < 2; j++)
				for (uint32_t k = 0; k < 3; k++)
					result[{i, j}] += a[{i, k}] * b[{k, j}];
		return result;
	}
	Float2x3 operator*(const Float2x3& a, const Float3x3& b)
	{
		Float2x3 result;
		for (uint32_t i = 0; i < 2; i++)
			for (uint32_t j = 0; j < 3; j++)
				for (uint32_t k = 0; k < 3; k++)
					result[{i, j}] += a[{i, k}] * b[{k, j}];
		return result;
	}
	Float2x3 operator*(const Float2x2& a, const Float2x3& b)
	{
		Float2x3 result;
		for (uint32_t i = 0; i < 2; i++)
			for (uint32_t j = 0; j < 3; j++)
				for (uint32_t k = 0; k < 2; k++)
					result[{i, j}] += a[{i, k}] * b[{k, j}];
		return result;
	}
	Float3x3 operator*(const Float3x2& a, const Float2x3& b)
	{
		Float3x3 result;
		for (uint32_t i = 0; i < 3; i++)
			for (uint32_t j = 0; j < 3; j++)
				for (uint32_t k = 0; k < 2; k++)
					result[{i, j}] += a[{i, k}] * b[{k, j}];
		return result;
	}



	// Logging:
	std::string Float2x3::ToString() const
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1];
		oss << " | " << data[2] << ", " << data[3];
		oss << " | " << data[4] << ", " << data[5] << ")";
		return oss.str();
	}
	std::string Float2x3::ToStringMatrixForm() const
	{
		std::ostringstream oss;
		oss << "\n";
		oss << "(" << data[0] << ", " << data[2] << ", " << data[4] << ")\n";
		oss << "(" << data[1] << ", " << data[3] << ", " << data[5] << ")";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Float2x3& value)
	{
		os << value.ToString();
		return os;
	}



	// Static members:
	Float2x3 Float2x3::zero = Float2x3(0.0f);
	Float2x3 Float2x3::maxValue = Float2x3(math::maxValue);
	Float2x3 Float2x3::minValue = Float2x3(math::minValue);
}