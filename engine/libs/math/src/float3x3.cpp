#include "float3x3.h"
#include "float3.h"
#include "float4x4.h"
#include "geometry3d.h"
#include "mathFunctions.h"
#include "uint3.h"
#include <stdexcept>
#include <sstream>



namespace emberMath
{
	// Constructors:
	Float3x3::Float3x3
	(float xx, float xy, float xz,	// column 0
		float yx, float yy, float yz,	// column 1
		float zx, float zy, float zz)	// column 2
	{
		data[0] = xx; data[3] = yx; data[6] = zx;
		data[1] = xy; data[4] = yy; data[7] = zy;
		data[2] = xz; data[5] = yz; data[8] = zz;
	}
	Float3x3::Float3x3()
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] = 0.0f;
	}
	Float3x3::Float3x3(float value)
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] = value;
	}
	Float3x3::Float3x3(const float* const array)
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] = array[i];
	}
	Float3x3::Float3x3(const Float3x3& other)
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] = other.data[i];
	}
	Float3x3::Float3x3(const Float4x4& other)
	{
		data[0] = other.data[0]; data[3] = other.data[4]; data[6] = other.data[8];
		data[1] = other.data[1]; data[4] = other.data[5]; data[7] = other.data[9];
		data[2] = other.data[2]; data[5] = other.data[6]; data[8] = other.data[10];
	}



	// Static constructors:
	Float3x3 Float3x3::Rows(const Float3& row0, const Float3& row1, const Float3& row2)
	{
		return Float3x3
		(row0.x, row1.x, row2.x,
			row0.y, row1.y, row2.y,
			row0.z, row1.z, row2.z);
	}
	Float3x3 Float3x3::Rows
	(float row0x, float row0y, float row0z,
		float row1x, float row1y, float row1z,
		float row2x, float row2y, float row2z)
	{
		return Float3x3
		(row0x, row1x, row2x,
			row0y, row1y, row2y,
			row0z, row1z, row2z);
	}
	Float3x3 Float3x3::Columns(const Float3& column0, const Float3& column1, const Float3& column2)
	{
		return Float3x3
		(column0.x, column0.y, column0.z,
			column1.x, column1.y, column1.z,
			column2.x, column2.y, column2.z);
	}
	Float3x3 Float3x3::Columns
	(float column0x, float column0y, float column0z,
		float column1x, float column1y, float column1z,
		float column2x, float column2y, float column2z)
	{
		return Float3x3
		(column0x, column0y, column0z,
			column1x, column1y, column1z,
			column2x, column2y, column2z);
	}



	// Math operations:
	Float3x3 Float3x3::Transpose() const
	{
		return Float3x3
		(data[0], data[3], data[6],
		 data[1], data[4], data[7],
		 data[2], data[5], data[8]);
	}
	float Float3x3::Determinant() const
	{
		return data[0] * (data[4] * data[8] - data[5] * data[7])
			 - data[1] * (data[3] * data[8] - data[5] * data[6])
			 + data[2] * (data[3] * data[7] - data[4] * data[6]);
	}
	Float3x3 Float3x3::Inverse() const
	{
		float det = Determinant();
		return Inverse(det);
	}
	Float3x3 Float3x3::Inverse(float det) const
	{
		if (det == 0.0f)
			return Float3x3::zero;
		float invDet = 1.0f / det;
		return Float3x3::Columns
		(
			(data[4] * data[8] - data[5] * data[7]) * invDet,
			(data[2] * data[7] - data[1] * data[8]) * invDet,
			(data[1] * data[5] - data[2] * data[4]) * invDet,

			(data[5] * data[6] - data[3] * data[8]) * invDet,
			(data[0] * data[8] - data[2] * data[6]) * invDet,
			(data[2] * data[3] - data[0] * data[5]) * invDet,

			(data[3] * data[7] - data[4] * data[6]) * invDet,
			(data[1] * data[6] - data[0] * data[7]) * invDet,
			(data[0] * data[4] - data[1] * data[3]) * invDet
		);
	}
	bool Float3x3::IsEpsilonZero() const
	{
		return IsEpsilonEqual(Float3x3::zero);
	}



	// Static math operations:
	Float3x3 Float3x3::RotateX(float angle)
	{
		float c = math::Cos(angle);
		float s = math::Sin(angle);
		return Float3x3::Rows
		(1.0f, 0.0f, 0.0f,
		 0.0f, c, -s,
		 0.0f, s, c);
	}
	Float3x3 Float3x3::RotateY(float angle)
	{
		float c = math::Cos(angle);
		float s = math::Sin(angle);
		return Float3x3::Rows
		(c, 0.0f, s,
		 0.0f, 1.0f, 0.0f,
		 -s, 0.0f, c);
	}
	Float3x3 Float3x3::RotateZ(float angle)
	{
		float c = math::Cos(angle);
		float s = math::Sin(angle);
		return Float3x3::Rows
		(c, -s, 0.0f,
		 s, c, 0.0f,
		 0.0f, 0.0f, 1.0f);
	}
	Float3x3 Float3x3::Rotate(const Float3& axis, float angle)
	{
		float c = math::Cos(angle);
		float s = math::Sin(angle);
		float t = 1.0f - c;
		Float3 normalizedAxis = axis.Normalize();
		float x = normalizedAxis.x;
		float y = normalizedAxis.y;
		float z = normalizedAxis.z;
		return Float3x3::Rows
		(x * x * t + c, x * y * t - z * s, x * z * t + y * s,
		 y * x * t + z * s, y * y * t + c, y * z * t - x * s,
		 z * x * t - y * s, z * y * t + x * s, z * z * t + c);
	}
	Float3x3 Float3x3::Rotate(const Float3& angles, const Uint3& rotationOrder, CoordinateSystem rotationSystem)
	{
		Float3x3 rot[3] = { RotateX(angles.x), RotateY(angles.y), RotateZ(angles.z) };
		if (rotationSystem == CoordinateSystem::local)
			return rot[rotationOrder.x] * rot[rotationOrder.y] * rot[rotationOrder.z];
		// (rotationSystem == CoordinateSystem::World)
		return rot[rotationOrder.z] * rot[rotationOrder.y] * rot[rotationOrder.x];
	}
	Float3x3 Float3x3::RotateFromTo(const Float3& from, const Float3& to)
	{
		Float3 f = from.Normalize();
		Float3 t = to.Normalize();
		if (f.IsEpsilonEqual(t))
			return Float3x3::identity;
		if (f.IsEpsilonEqual(-t))
			return Float3x3::Rotate(geometry3d::GetOrhtogonalVector(f), math::pi);
		Float3 axis = Float3::Cross(f, t); // normalization not needed, as Rotate(...) will normalize it
		float angle = Float3::Angle(f, t);
		return Rotate(axis, angle);
	}
	Float3x3 Float3x3::RotateThreeLeg(const Float3& direction0Old, const Float3& direction0New, const Float3& direction1Old, const Float3& direction1New)
	{
		// Rotate direction0Old to direction0New:
		Float3 axis = Float3::Cross(direction0Old, direction0New);
		float angle0 = Float3::Angle(direction0Old, direction0New);
		Float3x3 rot0 = Rotate(axis, angle0);

		// Compute missalignment angle between direction1New and direction1Old rotated by rot0:
		Float3 otherOldRotated = rot0 * direction1Old;
		Float3 planeNormal = Float3::Cross(direction1New, direction0New).Normalize();
		Float3 projection = geometry3d::PointToPlaneProjection(otherOldRotated, Float3::zero, planeNormal);
		float sign = math::Sign(Float3::Dot(Float3::Cross(otherOldRotated, projection), direction0New));
		float angle1 = sign * Float3::Angle(otherOldRotated, projection);
		if (Float3::Dot(direction1New, otherOldRotated) < 0)
			angle1 += math::pi;

		// Rotate by angle around direction0New:
		Float3x3 rot1 = Float3x3::Rotate(direction0New, angle1);

		// Combine rotations:
		return rot1 * rot0;
	}



	// Access:
	float& Float3x3::operator[](const Index2& index)
	{
		if (index.i >= 0 && index.i < 3 && index.j >= 0 && index.j < 3)
			return data[index.i + 3 * index.j];
		throw std::out_of_range("Float3x3 index out of range.");
	}
	float Float3x3::operator[](const Index2& index) const
	{
		if (index.i >= 0 && index.i < 3 && index.j >= 0 && index.j < 3)
			return data[index.i + 3 * index.j];
		throw std::out_of_range("Float3x3 index out of range.");
	}
	Float3 Float3x3::GetRow(int index) const
	{
		if (index >= 0 && index < 3)
			return Float3(data[index], data[index + 3], data[index + 6]);
		throw std::out_of_range("Float3x3 row index out of range.");
	}
	Float3 Float3x3::GetColumn(int index) const
	{
		if (index >= 0 && index < 3)
			return Float3(data[3 * index], data[3 * index + 1], data[3 * index + 2]);
		throw std::out_of_range("Float3x3 column index out of range.");
	}



	// Assignment:
	Float3x3& Float3x3::operator=(const Float3x3& other)
	{
		if (this != &other)
		{
			for (uint32_t i = 0; i < 9; i++)
				data[i] = other.data[i];
		}
		return *this;
	}
	Float3x3& Float3x3::operator=(Float3x3&& other) noexcept
	{
		if (this != &other)
		{
			for (uint32_t i = 0; i < 9; i++)
				data[i] = other.data[i];
		}
		return *this;
	}



	// Addition:
	Float3x3 Float3x3::operator+(const Float3x3& other) const
	{
		Float3x3 result;
		for (uint32_t i = 0; i < 9; i++)
			result.data[i] = data[i] + other.data[i];
		return result;
	}
	Float3x3& Float3x3::operator+=(const Float3x3& other)
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] += other.data[i];
		return *this;
	}



	// Substraction:
	Float3x3 Float3x3::operator-(const Float3x3& other) const
	{
		Float3x3 result;
		for (uint32_t i = 0; i < 9; i++)
			result.data[i] = data[i] - other.data[i];
		return result;
	}
	Float3x3& Float3x3::operator-=(const Float3x3& other)
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] -= other.data[i];
		return *this;
	}
	Float3x3 Float3x3::operator-() const
	{
		return Float3x3
		(-data[0], -data[1], -data[2],
		 -data[3], -data[4], -data[5],
		 -data[6], -data[7], -data[8]);
	}



	// Multiplication:
	Float3x3 Float3x3::operator*(const Float3x3& other) const
	{
		Float3x3 result;
		for (uint32_t i = 0; i < 3; i++)
			for (uint32_t j = 0; j < 3; j++)
				for (uint32_t k = 0; k < 3; k++)
					result[{i, j}] += (*this)[{i, k}] * other[{k, j}];
		return result;
	}
	Float3x3& Float3x3::operator*=(const Float3x3& other)
	{
		Float3x3 result = (*this) * other;
		*this = result;
		return *this;
	}
	Float3x3& Float3x3::operator*=(float scalar)
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] *= scalar;
		return *this;
	}



	// Division:
	Float3x3 Float3x3::operator/(float scalar) const
	{
		Float3x3 result;
		for (uint32_t i = 0; i < 9; i++)
			result.data[i] = data[i] / scalar;
		return result;
	}
	Float3x3& Float3x3::operator/=(float scalar)
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] /= scalar;
		return *this;
	}



	// Comparison:
	bool Float3x3::IsEpsilonEqual(const Float3x3& other) const
	{
		for (uint32_t i = 0; i < 9; i++)
			if (math::Abs(data[i] - other.data[i]) > math::epsilon)
				return false;
		return true;
	}
	bool Float3x3::operator==(const Float3x3& other) const
	{
		for (uint32_t i = 0; i < 9; i++)
			if (data[i] != other.data[i])
				return false;
		return true;
	}
	bool Float3x3::operator!=(const Float3x3& other) const
	{
		return !(*this == other);
	}



	// Friend functions:
	Float3x3 operator*(const Float3x3& a, float b)
	{
		Float3x3 result;
		for (uint32_t i = 0; i < 9; i++)
			result.data[i] = a.data[i] * b;
		return result;
	}
	Float3x3 operator*(float a, const Float3x3& b)
	{
		Float3x3 result;
		for (uint32_t i = 0; i < 9; i++)
			result.data[i] = a * b.data[i];
		return result;
	}
	Float3 operator*(const Float3x3& a, const Float3& b)
	{
		return Float3
		(a.data[0] * b.x + a.data[3] * b.y + a.data[6] * b.z,
		 a.data[1] * b.x + a.data[4] * b.y + a.data[7] * b.z,
		 a.data[2] * b.x + a.data[5] * b.y + a.data[8] * b.z);
	}
	Float3 operator*(const Float3& a, const Float3x3& b)
	{
		return Float3
		(a.x * b.data[0] + a.y * b.data[1] + a.z * b.data[2],
		 a.x * b.data[3] + a.y * b.data[4] + a.z * b.data[5],
		 a.x * b.data[6] + a.y * b.data[7] + a.z * b.data[8]);
	}



	// Logging:
	std::string Float3x3::ToString() const
	{
		std::ostringstream oss;
		oss <<   "(" << data[0] << ", " << data[1] << ", " << data[2];
		oss << " | " << data[3] << ", " << data[4] << ", " << data[5];
		oss << " | " << data[6] << ", " << data[7] << ", " << data[8] << ")";
		return oss.str();
	}
	std::string Float3x3::ToStringMatrixForm() const
	{
		std::ostringstream oss;
		oss << "\n";
		oss << "(" << data[0] << ", " << data[3] << ", " << data[6] << ")\n";
		oss << "(" << data[1] << ", " << data[4] << ", " << data[7] << ")\n";
		oss << "(" << data[2] << ", " << data[5] << ", " << data[8] << ")";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Float3x3& value)
	{
		os << value.ToString();
		return os;
	}



	// Static members:
	// Numbers:
	Float3x3 Float3x3::zero = Float3x3(0.0f);
	Float3x3 Float3x3::identity = Float3x3
	(1.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 1.0f);
	Float3x3 Float3x3::maxValue = Float3x3(math::maxValue);
	Float3x3 Float3x3::minValue = Float3x3(math::minValue);

	// Rotations:
	Float3x3 Float3x3::rot45x = Float3x3::RotateX(math::pi4);
	Float3x3 Float3x3::rot45y = Float3x3::RotateY(math::pi4);
	Float3x3 Float3x3::rot45z = Float3x3::RotateZ(math::pi4);
	Float3x3 Float3x3::rot90x = Float3x3::RotateX(math::pi2);
	Float3x3 Float3x3::rot90y = Float3x3::RotateY(math::pi2);
	Float3x3 Float3x3::rot90z = Float3x3::RotateZ(math::pi2);
	Float3x3 Float3x3::rot135x = Float3x3::RotateX(3.0f * math::pi4);
	Float3x3 Float3x3::rot135y = Float3x3::RotateY(3.0f * math::pi4);
	Float3x3 Float3x3::rot135z = Float3x3::RotateZ(3.0f * math::pi4);
	Float3x3 Float3x3::rot180x = Float3x3::RotateX(math::pi);
	Float3x3 Float3x3::rot180y = Float3x3::RotateY(math::pi);
	Float3x3 Float3x3::rot180z = Float3x3::RotateZ(math::pi);
	Float3x3 Float3x3::rot225x = Float3x3::RotateX(5.0f * math::pi4);
	Float3x3 Float3x3::rot225y = Float3x3::RotateY(5.0f * math::pi4);
	Float3x3 Float3x3::rot225z = Float3x3::RotateZ(5.0f * math::pi4);
	Float3x3 Float3x3::rot270x = Float3x3::RotateX(-math::pi2);
	Float3x3 Float3x3::rot270y = Float3x3::RotateY(-math::pi2);
	Float3x3 Float3x3::rot270z = Float3x3::RotateZ(-math::pi2);
	Float3x3 Float3x3::rot315x = Float3x3::RotateX(-math::pi4);
	Float3x3 Float3x3::rot315y = Float3x3::RotateY(-math::pi4);
	Float3x3 Float3x3::rot315z = Float3x3::RotateZ(-math::pi4);
}