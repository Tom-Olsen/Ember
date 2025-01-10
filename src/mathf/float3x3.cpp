#include "float3x3.h"
#include "float3.h"
#include "float4x4.h"
#include "geometry3d.h"
#include "logger.h"
#include "mathf.h"
#include "uint3.h"
#include <stdexcept>
#include <sstream>



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
		data[i] = other[i];
}
Float3x3::Float3x3(const Float4x4& other)
{
	data[0] = other[0]; data[3] = other[4]; data[6] = other[8];
	data[1] = other[1]; data[4] = other[5]; data[7] = other[9];
	data[2] = other[2]; data[5] = other[6]; data[8] = other[10];
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
	{
		LOG_WARN("Float3x3::Inverse(), determinant is zero.");
			return Float3x3::zero;
	}
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
	float c = mathf::Cos(angle);
	float s = mathf::Sin(angle);
	return Float3x3::Rows
	(1.0f, 0.0f, 0.0f,
	 0.0f, c, -s,
	 0.0f, s, c);
}
Float3x3 Float3x3::RotateY(float angle)
{
	float c = mathf::Cos(angle);
	float s = mathf::Sin(angle);
	return Float3x3::Rows
	(c, 0.0f, s,
	 0.0f, 1.0f, 0.0f,
	 -s, 0.0f, c);
}
Float3x3 Float3x3::RotateZ(float angle)
{
	float c = mathf::Cos(angle);
	float s = mathf::Sin(angle);
	return Float3x3::Rows
	(c, -s, 0.0f,
	 s, c, 0.0f,
	 0.0f, 0.0f, 1.0f);
}
Float3x3 Float3x3::Rotate(const Float3& axis, float angle)
{
	float c = mathf::Cos(angle);
	float s = mathf::Sin(angle);
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
	Float3 axis = Float3::Cross(f, t).Normalize();
	float angle = Float3::Angle(f, t);
	return Rotate(axis, angle);
}
Float3x3 Float3x3::RotateThreeLeg(const Float3& forwardOld, const Float3& forwardNew, const Float3& otherOld, const Float3& otherNew)
{
	// Rotate forwardOld to forwardNew:
	Float3 axis = Float3::Cross(forwardOld, forwardNew);
	float angle0 = Float3::Angle(forwardOld, forwardNew);
	Float3x3 rot0 = Rotate(axis, angle0);

	// Compute missalignment angle between otherNew and otherOld rotated by rot0:
	Float3 otherOldRotated = rot0 * otherOld;
	Float3 planeNormal = Float3::Cross(otherNew, forwardNew).Normalize();
	Float3 projection = geometry3d::PointToPlaneProjection(otherOldRotated, Float3::zero, planeNormal);
	float sign = mathf::Sign(Float3::Dot(Float3::Cross(otherOldRotated, projection), forwardNew));
	float angle1 = sign * Float3::Angle(otherOldRotated, projection);
	if (Float3::Dot(otherNew, otherOldRotated) < 0)
		angle1 += mathf::pi;

	// Rotate by angle around forwardNew:
	Float3x3 rot1 = Float3x3::Rotate(forwardNew, angle1);

	// Combine rotations:
	return rot1 * rot0;
}



// Access:
float& Float3x3::operator[](int index)
{
	if (index >= 0 && index < 9)
		return data[index];
	throw std::out_of_range("Float3x3 index out of range.");
}
float Float3x3::operator[](int index) const
{
	if (index >= 0 && index < 9)
		return data[index];
	throw std::out_of_range("Float3x3 index out of range.");
}
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
			data[i] = other[i];
	}
	return *this;
}
Float3x3& Float3x3::operator=(Float3x3&& other) noexcept
{
	if (this != &other)
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] = other[i];
	}
	return *this;
}



// Addition:
Float3x3 Float3x3::operator+(const Float3x3& other) const
{
	Float3x3 result;
	for (uint32_t i = 0; i < 9; i++)
		result[i] = data[i] + other[i];
	return result;
}
Float3x3& Float3x3::operator+=(const Float3x3& other)
{
	for (uint32_t i = 0; i < 9; i++)
		data[i] += other[i];
	return *this;
}



// Substraction:
Float3x3 Float3x3::operator-(const Float3x3& other) const
{
	Float3x3 result;
	for (uint32_t i = 0; i < 9; i++)
		result[i] = data[i] - other[i];
	return result;
}
Float3x3& Float3x3::operator-=(const Float3x3& other)
{
	for (uint32_t i = 0; i < 9; i++)
		data[i] -= other[i];
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
		result[i] = data[i] / scalar;
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
		if (mathf::Abs(data[i] - other[i]) > mathf::epsilon)
			return false;
	return true;
}
bool Float3x3::operator==(const Float3x3& other) const
{
	for (uint32_t i = 0; i < 9; i++)
		if (data[i] != other[i])
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
		result[i] = a[i] * b;
	return result;
}
Float3x3 operator*(float a, const Float3x3& b)
{
	Float3x3 result;
	for (uint32_t i = 0; i < 9; i++)
		result[i] = a * b[i];
	return result;
}
Float3 operator*(const Float3x3& a, const Float3& b)
{
	return Float3
	(a[0] * b.x + a[3] * b.y + a[6] * b.z,
	 a[1] * b.x + a[4] * b.y + a[7] * b.z,
	 a[2] * b.x + a[5] * b.y + a[8] * b.z);
}
Float3 operator*(const Float3& a, const Float3x3& b)
{
	return Float3
	(a.x * b[0] + a.y * b[1] + a.z * b[2],
	 a.x * b[3] + a.y * b[4] + a.z * b[5],
	 a.x * b[6] + a.y * b[7] + a.z * b[8]);
}



// Logging:
std::string Float3x3::ToString() const
{
	std::ostringstream oss;
	oss << "("   << data[0] << ", " << data[1] << ", " << data[2];
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
Float3x3 Float3x3::max = Float3x3(mathf::max);
Float3x3 Float3x3::min = Float3x3(mathf::min);

// Rotations:
Float3x3 Float3x3::rot90x = Float3x3::RotateX(mathf::pi2);
Float3x3 Float3x3::rot90y = Float3x3::RotateY(mathf::pi2);
Float3x3 Float3x3::rot90z = Float3x3::RotateZ(mathf::pi2);
Float3x3 Float3x3::rot180x = Float3x3::RotateX(mathf::pi);
Float3x3 Float3x3::rot180y = Float3x3::RotateY(mathf::pi);
Float3x3 Float3x3::rot180z = Float3x3::RotateZ(mathf::pi);
Float3x3 Float3x3::rot270x = Float3x3::RotateX(-mathf::pi2);
Float3x3 Float3x3::rot270y = Float3x3::RotateY(-mathf::pi2);
Float3x3 Float3x3::rot270z = Float3x3::RotateZ(-mathf::pi2);