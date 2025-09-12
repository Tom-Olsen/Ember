#include "float3x2.h"
#include "float2.h"
#include "float3.h"
#include "float2x2.h"
#include "float2x3.h"
#include "float3x3.h"
#include "mathFunctions.h"
#include <stdexcept>
#include <sstream>



namespace emberMath
{
	// Constructors:
	Float3x2::Float3x2
	(float xx, float xy, float xz,	// column 0
		float yx, float yy, float yz)	// column 1
	{
		data[0] = xx; data[3] = yx;
		data[1] = xy; data[4] = yy;
		data[2] = xz; data[5] = yz;
	}
	Float3x2::Float3x2()
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] = 0.0f;
	}
	Float3x2::Float3x2(float value)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] = value;
	}
	Float3x2::Float3x2(const float* const array)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] = array[i];
	}
	Float3x2::Float3x2(const Float3x2& other)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] = other.data[i];
	}



	// Static constructors:
	Float3x2 Float3x2::Rows(const Float2& row0, const Float2& row1, const Float2& row2)
	{
		return Float3x2
		(row0.x, row1.x, row2.x,
			row0.y, row1.y, row2.y);
	}
	Float3x2 Float3x2::Rows
	(float row0x, float row0y,
		float row1x, float row1y,
		float row2x, float row2y)
	{
		return Float3x2
		(row0x, row1x, row2x,
			row0y, row1y, row2y);
	}
	Float3x2 Float3x2::Columns(const Float3& column0, const Float3& column1)
	{
		return Float3x2
		(column0.x, column0.y, column0.z,
			column1.x, column1.y, column1.z);
	}
	Float3x2 Float3x2::Columns
	(float column0x, float column0y, float column0z,
		float column1x, float column1y, float column1z)
	{
		return Float3x2
		(column0x, column0y, column0z,
			column1x, column1y, column1z);
	}



	// Math operations:
	Float2x3 Float3x2::Transpose() const
	{
		return Float2x3::Rows
		(data[0], data[1], data[2],
		 data[3], data[4], data[5]);
	}
	Float2x3 Float3x2::LeftInverse() const
	{
		Float2x3 aT = this->Transpose();
		return (aT * (*this)).Inverse() * aT;
	}
	bool Float3x2::IsEpsilonZero() const
	{
		return IsEpsilonEqual(Float3x2::zero);
	}



	// Access:
	float& Float3x2::operator[](const Index2& index)
	{
		if (index.i >= 0 && index.i < 3 && index.j >= 0 && index.j < 2)
			return data[index.i + 3 * index.j];
		throw std::out_of_range("Float3x2 index out of range.");
	}
	float Float3x2::operator[](const Index2& index) const
	{
		if (index.i >= 0 && index.i < 3 && index.j >= 0 && index.j < 2)
			return data[index.i + 3 * index.j];
		throw std::out_of_range("Float3x2 index out of range.");
	}
	Float2 Float3x2::GetRow(int index) const
	{
		if (index >= 0 && index < 3)
			return Float2(data[index], data[index + 3]);
		throw std::out_of_range("Float3x2 row index out of range.");
	}
	Float3 Float3x2::GetColumn(int index) const
	{
		if (index >= 0 && index < 2)
			return Float3(data[3 * index], data[3 * index + 1], data[3 * index + 2]);
		throw std::out_of_range("Float3x2 column index out of range.");
	}



	// Assignment:
	Float3x2& Float3x2::operator=(const Float3x2& other)
	{
		if (this != &other)
		{
			for (uint32_t i = 0; i < 6; i++)
				data[i] = other.data[i];
		}
		return *this;
	}
	Float3x2& Float3x2::operator=(Float3x2&& other) noexcept
	{
		if (this != &other)
		{
			for (uint32_t i = 0; i < 6; i++)
				data[i] = other.data[i];
		}
		return *this;
	}



	// Addition:
	Float3x2 Float3x2::operator+(const Float3x2& other) const
	{
		Float3x2 result;
		for (uint32_t i = 0; i < 6; i++)
			result.data[i] = data[i] + other.data[i];
		return result;
	}
	Float3x2& Float3x2::operator+=(const Float3x2& other)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] += other.data[i];
		return *this;
	}



	// Substraction:
	Float3x2 Float3x2::operator-(const Float3x2& other) const
	{
		Float3x2 result;
		for (uint32_t i = 0; i < 6; i++)
			result.data[i] = data[i] - other.data[i];
		return result;
	}
	Float3x2& Float3x2::operator-=(const Float3x2& other)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] -= other.data[i];
		return *this;
	}
	Float3x2 Float3x2::operator-() const
	{
		return Float3x2
		(-data[0], -data[1], -data[2],
		 -data[3], -data[4], -data[5]);
	}



	// Multiplication:
	Float3x2& Float3x2::operator*=(float scalar)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] *= scalar;
		return *this;
	}



	// Division:
	Float3x2 Float3x2::operator/(float scalar) const
	{
		Float3x2 result;
		for (uint32_t i = 0; i < 6; i++)
			result.data[i] = data[i] / scalar;
		return result;
	}
	Float3x2& Float3x2::operator/=(float scalar)
	{
		for (uint32_t i = 0; i < 6; i++)
			data[i] /= scalar;
		return *this;
	}



	// Comparison:
	bool Float3x2::IsEpsilonEqual(const Float3x2& other) const
	{
		for (uint32_t i = 0; i < 6; i++)
			if (math::Abs(data[i] - other.data[i]) > math::epsilon)
				return false;
		return true;
	}
	bool Float3x2::operator==(const Float3x2& other) const
	{
		for (uint32_t i = 0; i < 6; i++)
			if (data[i] != other.data[i])
				return false;
		return true;
	}
	bool Float3x2::operator!=(const Float3x2& other) const
	{
		return !(*this == other);
	}



	// Friend functions:
	Float3x2 operator*(const Float3x2& a, float b)
	{
		Float3x2 result;
		for (uint32_t i = 0; i < 6; i++)
			result.data[i] = a.data[i] * b;
		return result;
	}
	Float3x2 operator*(float a, const Float3x2& b)
	{
		Float3x2 result;
		for (uint32_t i = 0; i < 6; i++)
			result.data[i] = a * b.data[i];
		return result;
	}
	Float3 operator*(const Float3x2& a, const Float2& b)
	{
		return Float3
		(a.data[0] * b.x + a.data[3] * b.y,
		 a.data[1] * b.x + a.data[4] * b.y,
		 a.data[2] * b.x + a.data[5] * b.y);
	}
	Float2 operator*(const Float3& a, const Float3x2& b)
	{
		return Float2
		(a.x * b.data[0] + a.y * b.data[1] + a.z * b.data[2],
		 a.x * b.data[3] + a.y * b.data[4] + a.z * b.data[5]);
	}
	Float3x2 operator*(const Float3x2& a, const Float2x2& b)
	{
		Float3x2 result;
		for (uint32_t i = 0; i < 3; i++)
			for (uint32_t j = 0; j < 2; j++)
				for (uint32_t k = 0; k < 2; k++)
					result[{i, j}] += a[{i, k}] * b[{k, j}];
		return result;
	}
	Float3x2 operator*(const Float3x3& a, const Float3x2& b)
	{
		Float3x2 result;
		for (uint32_t i = 0; i < 3; i++)
			for (uint32_t j = 0; j < 2; j++)
				for (uint32_t k = 0; k < 3; k++)
					result[{i, j}] += a[{i, k}] * b[{k, j}];
		return result;
	}



	// Logging:
	std::string Float3x2::ToString() const
	{
		std::ostringstream oss;
		oss <<   "(" << data[0] << ", " << data[1] << ", " << data[2];
		oss << " | " << data[3] << ", " << data[4] << ", " << data[5] << ")";
		return oss.str();
	}
	std::string Float3x2::ToStringMatrixForm() const
	{
		std::ostringstream oss;
		oss << "\n";
		oss << "(" << data[0] << ", " << data[3] << ")\n";
		oss << "(" << data[1] << ", " << data[4] << ")\n";
		oss << "(" << data[2] << ", " << data[5] << ")";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Float3x2& value)
	{
		os << value.ToString();
		return os;
	}



	// Static members:
	Float3x2 Float3x2::zero = Float3x2(0.0f);
	Float3x2 Float3x2::maxValue = Float3x2(math::maxValue);
	Float3x2 Float3x2::minValue = Float3x2(math::minValue);
}