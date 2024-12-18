#include "float4x4.h"
#include "float4.h"
#include "float3x3.h"
#include "logger.h"
#include "uint3.h"
#include <stdexcept>
#include <sstream>



// Constructors:
Float4x4::Float4x4()
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] = 0.0f;
}
Float4x4::Float4x4(float value)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] = value;
}
Float4x4::Float4x4(const float* array)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] = array[i];
}
Float4x4::Float4x4
(float xx, float xy, float xz, float xw,	// column 0
	float yx, float yy, float yz, float yw,	// column 1
	float zx, float zy, float zz, float zw,	// column 2
	float wx, float wy, float wz, float ww)	// column 3
{
	data[0] = xx; data[4] = yx; data[8] = zx; data[12] = wx;
	data[1] = xy; data[5] = yy; data[9] = zy; data[13] = wy;
	data[2] = xz; data[6] = yz; data[10] = zz; data[14] = wz;
	data[3] = xw; data[7] = yw; data[11] = zw; data[15] = ww;
}
Float4x4::Float4x4(const Float3x3& other)
{
	data[0] = other[0]; data[4] = other[3]; data[8] = other[6]; data[12] = 0.0f;
	data[1] = other[1]; data[5] = other[4]; data[9] = other[7]; data[13] = 0.0f;
	data[2] = other[2]; data[6] = other[5]; data[10] = other[8]; data[14] = 0.0f;
	data[3] = 0.0f; data[7] = 0.0f; data[11] = 0.0f; data[15] = 1.0f;
}
Float4x4::Float4x4(const Float4x4& other)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] = other.data[i];
}



// Static constructors:
Float4x4 Float4x4::Rows(const Float4& row0, const Float4& row1, const Float4& row2, const Float4& row3)
{
	return Float4x4
	(row0.x, row1.x, row2.x, row3.x,
	 row0.y, row1.y, row2.y, row3.y,
	 row0.z, row1.z, row2.z, row3.z,
	 row0.w, row1.w, row2.w, row3.w);
}
Float4x4 Float4x4::Rows
(float row0x, float row0y, float row0z, float row0w,
 float row1x, float row1y, float row1z, float row1w,
 float row2x, float row2y, float row2z, float row2w,
 float row3x, float row3y, float row3z, float row3w)
{
	return Float4x4
	(row0x, row1x, row2x, row3x,
	 row0y, row1y, row2y, row3y,
	 row0z, row1z, row2z, row3z,
	 row0w, row1w, row2w, row3w);
}
Float4x4 Float4x4::Columns(const Float4& column0, const Float4& column1, const Float4& column2, const Float4& column3)
{
	return Float4x4
	(column0.x, column0.y, column0.z, column0.w,
	 column1.x, column1.y, column1.z, column1.w,
	 column2.x, column2.y, column2.z, column2.w,
	 column3.x, column3.y, column3.z, column3.w);
}
Float4x4 Float4x4::Columns
(float column0x, float column0y, float column0z, float column0w,
 float column1x, float column1y, float column1z, float column1w,
 float column2x, float column2y, float column2z, float column2w,
 float column3x, float column3y, float column3z, float column3w)
{
	return Float4x4
	(column0x, column0y, column0z, column0w,
		column1x, column1y, column1z, column1w,
		column2x, column2y, column2z, column2w,
		column3x, column3y, column3z, column3w);
}



// Math operations:
Float4x4 Float4x4::Transpose()
{
	return Float4x4
	(data[0], data[4], data[8], data[12],
	 data[1], data[5], data[9], data[13],
	 data[2], data[6], data[10], data[14],
	 data[3], data[7], data[11], data[15]);
}
float Float4x4::Determinant() const
{
	return data[0] * (data[5] * (data[10] * data[15] - data[11] * data[14]) - data[6] * (data[9] * data[15] - data[11] * data[13]) + data[7] * (data[9] * data[14] - data[10] * data[13]))
		- data[1] * (data[4] * (data[10] * data[15] - data[11] * data[14]) - data[6] * (data[8] * data[15] - data[11] * data[12]) + data[7] * (data[8] * data[14] - data[10] * data[12]))
		+ data[2] * (data[4] * (data[9] * data[15] - data[11] * data[13]) - data[5] * (data[8] * data[15] - data[11] * data[12]) + data[7] * (data[8] * data[13] - data[9] * data[12]))
		- data[3] * (data[4] * (data[9] * data[14] - data[10] * data[13]) - data[5] * (data[8] * data[14] - data[10] * data[12]) + data[6] * (data[8] * data[13] - data[9] * data[12]));
}
Float4x4 Float4x4::Inverse() const
{
	float det = Determinant();
	return Inverse(det);
}
Float4x4 Float4x4::Inverse(float det) const
{
	if (det == 0.0f)
	{
		LOG_WARN("Float4x4::Inverse(), determinant is zero.");
			return Float4x4::zero;
	}
	float invDet = 1.0f / det;
	return Float4x4::Columns
	(// column 0:
		(data[5] * (data[10] * data[15] - data[11] * data[14]) - data[6] * (data[9] * data[15] - data[11] * data[13]) + data[7] * (data[9] * data[14] - data[10] * data[13])) * invDet,
		-(data[1] * (data[10] * data[15] - data[11] * data[14]) - data[2] * (data[9] * data[15] - data[11] * data[13]) + data[3] * (data[9] * data[14] - data[10] * data[13])) * invDet,
		(data[1] * (data[6] * data[15] - data[7] * data[14]) - data[2] * (data[5] * data[15] - data[7] * data[13]) + data[3] * (data[5] * data[14] - data[6] * data[13])) * invDet,
		-(data[1] * (data[6] * data[11] - data[7] * data[10]) - data[2] * (data[5] * data[11] - data[7] * data[9]) + data[3] * (data[5] * data[10] - data[6] * data[9])) * invDet,
		// column 1:
		-(data[4] * (data[10] * data[15] - data[11] * data[14]) - data[6] * (data[8] * data[15] - data[11] * data[12]) + data[7] * (data[8] * data[14] - data[10] * data[12])) * invDet,
		(data[0] * (data[10] * data[15] - data[11] * data[14]) - data[2] * (data[8] * data[15] - data[11] * data[12]) + data[3] * (data[8] * data[14] - data[10] * data[12])) * invDet,
		-(data[0] * (data[6] * data[15] - data[7] * data[14]) - data[2] * (data[4] * data[15] - data[7] * data[12]) + data[3] * (data[4] * data[14] - data[6] * data[12])) * invDet,
		(data[0] * (data[6] * data[11] - data[7] * data[10]) - data[2] * (data[4] * data[11] - data[7] * data[8]) + data[3] * (data[4] * data[10] - data[6] * data[8])) * invDet,
		// column 2:
		(data[4] * (data[9] * data[15] - data[11] * data[13]) - data[5] * (data[8] * data[15] - data[11] * data[12]) + data[7] * (data[8] * data[13] - data[9] * data[12])) * invDet,
		-(data[0] * (data[9] * data[15] - data[11] * data[13]) - data[1] * (data[8] * data[15] - data[11] * data[12]) + data[3] * (data[8] * data[13] - data[9] * data[12])) * invDet,
		(data[0] * (data[5] * data[15] - data[7] * data[13]) - data[1] * (data[4] * data[15] - data[7] * data[12]) + data[3] * (data[4] * data[13] - data[5] * data[12])) * invDet,
		-(data[0] * (data[5] * data[11] - data[7] * data[9]) - data[1] * (data[4] * data[11] - data[7] * data[8]) + data[3] * (data[4] * data[9] - data[5] * data[8])) * invDet,
		// column 3:
		-(data[4] * (data[9] * data[14] - data[10] * data[13]) - data[5] * (data[8] * data[14] - data[10] * data[12]) + data[6] * (data[8] * data[13] - data[9] * data[12])) * invDet,
		(data[0] * (data[9] * data[14] - data[10] * data[13]) - data[1] * (data[8] * data[14] - data[10] * data[12]) + data[2] * (data[8] * data[13] - data[9] * data[12])) * invDet,
		-(data[0] * (data[5] * data[14] - data[6] * data[13]) - data[1] * (data[4] * data[14] - data[6] * data[12]) + data[2] * (data[4] * data[13] - data[5] * data[12])) * invDet,
		(data[0] * (data[5] * data[10] - data[6] * data[9]) - data[1] * (data[4] * data[10] - data[6] * data[8]) + data[2] * (data[4] * data[9] - data[5] * data[8])) * invDet
	);
}
bool Float4x4::IsEpsilonZero() const
{
	for (uint32_t i = 0; i < 16; i++)
		if (std::fabs(data[i]) > epsilon)
			return false;
	return true;
}



// Static math operations:
Float4x4 Float4x4::RotateX(float radians)
{
	float c = mathf::Cos(radians);
	float s = mathf::Sin(radians);
	return Float4x4::Rows
	(1.0f, 0.0f, 0.0f, 0.0f,
	 0.0f, c, -s, 0.0f,
	 0.0f, s, c, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::RotateY(float radians)
{
	float c = mathf::Cos(radians);
	float s = mathf::Sin(radians);
	return Float4x4::Rows
	(c, 0.0f, s, 0.0f,
	 0.0f, 1.0f, 0.0f, 0.0f,
	 -s, 0.0f, c, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::RotateZ(float radians)
{
	float c = mathf::Cos(radians);
	float s = mathf::Sin(radians);
	return Float4x4::Rows
	(c, -s, 0.0f, 0.0f,
	 s, c, 0.0f, 0.0f,
	 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::Rotate(const Float3& axis, float radians)
{
	float c = mathf::Cos(radians);
	float s = mathf::Sin(radians);
	float t = 1.0f - c;
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	return Float4x4::Rows
	(x * x * t + c, x * y * t - z * s, x * z * t + y * s, 0.0f,
	 y * x * t + z * s, y * y * t + c, y * z * t - x * s, 0.0f,
	 z * x * t - y * s, z * y * t + x * s, z * z * t + c, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::Rotate(const Float3& eulerRadians, const Uint3& rotationOrder, CoordinateSystem rotationSystem)
{
	Float4x4 rot[3] = { RotateX(eulerRadians.x), RotateY(eulerRadians.y), RotateZ(eulerRadians.z) };
	if (rotationSystem == CoordinateSystem::local)
		return rot[rotationOrder.x] * rot[rotationOrder.y] * rot[rotationOrder.z];
	else if (rotationSystem == CoordinateSystem::world)
		return rot[rotationOrder.z] * rot[rotationOrder.y] * rot[rotationOrder.x];
	return identity;
}
Float4x4 Float4x4::RotateFromTo(const Float3& from, const Float3& to)
{
	Float3 axis = Float3::Cross(from, to);
	float radians = Float3::AngleRadians(from, to);
	return Rotate(axis, radians);
}
Float4x4 Float4x4::RotateThreeLeg(const Float3& forwardOld, const Float3& forwardNew, const Float3& upOld, const Float3& upNew)
{
	// Rotate forwardOld to forwardNew:
	Float3 axis = Float3::Cross(forwardOld, forwardNew);
	float radians = Float3::AngleRadians(forwardOld, forwardNew);
	Float3x3 rot0 = Float3x3::Rotate(axis, radians);

	// Compute missalignment angle between upNew and upOld rotated by rot0:
	Float3 upOldRotated = rot0 * upOld;
	Float3 planeNormal = Float3::Cross(upNew, forwardNew).Normalize();
	Float3 projection = Float3::VectorToPlaneProjection(upOldRotated, planeNormal);
	float sign = mathf::Sign(Float3::Dot(Float3::Cross(upOldRotated, projection), forwardNew));
	float angle = sign * Float3::AngleRadians(upOldRotated, projection);
	if (Float3::Dot(upNew, upOldRotated) < 0)
		angle += mathf::PI;

	// Rotate by angle around forwardNew:
	Float3x3 rot1 = Float3x3::Rotate(forwardNew, angle);

	// Combine rotations:
	return Float4x4(rot1 * rot0);
}
Float4x4 Float4x4::Translate(const Float3& translation)
{
	return Float4x4::Rows
	(1.0f, 0.0f, 0.0f, translation.x,
		0.0f, 1.0f, 0.0f, translation.y,
		0.0f, 0.0f, 1.0f, translation.z,
		0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::Scale(const Float3& scale)
{
	return Float4x4::Rows
	(scale.x, 0.0f, 0.0f, 0.0f,
	 0.0f, scale.y, 0.0f, 0.0f,
	 0.0f, 0.0f, scale.z, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::Scale(float scale)
{
	return Scale(Float3(scale));
}
Float4x4 Float4x4::Perspective(float fovRadians, float aspectRatio, float nearClip, float farClip)
{
	float tanHalfFov = mathf::Tan(0.5f * fovRadians);
	float xx = 1.0f / (aspectRatio * tanHalfFov);
	float yy = -1.0f / tanHalfFov;
	float zz = (farClip + nearClip) / (nearClip - farClip);
	float wz = 2.0f * farClip * nearClip / (nearClip - farClip);

	return Float4x4::Rows
	(xx, 0.0f, 0.0f, 0.0f,
	 0.0f, yy, 0.0f, 0.0f,
	 0.0f, 0.0f, zz, wz,
	 0.0f, 0.0f, -1.0f, 0.0f);
}
Float4x4 Float4x4::Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip)
{
	float xx = 2.0f / (right - left);
	float yy = 2.0f / (top - bottom);
	float zz = -2.0f / (farClip - nearClip);
	float wx = -(right + left) / (right - left);
	float wy = -(top + bottom) / (top - bottom);
	float wz = -(farClip + nearClip) / (farClip - nearClip);

	return Float4x4::Rows
	(xx, 0.0f, 0.0f, wx,
	 0.0f, yy, 0.0f, wy,
	 0.0f, 0.0f, zz, wz,
	 0.0f, 0.0f, 0.0f, 1.0f);
}



// Access:
float& Float4x4::operator[](int index)
{
	if (index >= 0 && index < 16)
		return data[index];
	throw std::out_of_range("Float4x4 index out of range.");
}
float Float4x4::operator[](int index) const
{
	if (index >= 0 && index < 16)
		return data[index];
	throw std::out_of_range("Float4x4 index out of range.");
}
float& Float4x4::operator[](const Index2& index)
{
	if (index.i >= 0 && index.i < 4 && index.j >= 0 && index.j < 4)
		return data[index.i + 4 * index.j];
	throw std::out_of_range("Float4x4 index out of range.");
}
float Float4x4::operator[](const Index2& index) const
{
	if (index.i >= 0 && index.i < 4 && index.j >= 0 && index.j < 4)
		return data[index.i + 4 * index.j];
	throw std::out_of_range("Float4x4 index out of range.");
}
Float4 Float4x4::GetRow(int index) const
{
	if (index >= 0 && index < 4)
		return Float4(data[index], data[index + 4], data[index + 8], data[index + 12]);
	throw std::out_of_range("Float4x4 row index out of range.");
}
Float4 Float4x4::GetColumn(int index) const
{
	if (index >= 0 && index < 4)
		return Float4(data[4 * index], data[4 * index + 1], data[4 * index + 2], data[4 * index + 3]);
	throw std::out_of_range("Float4x4 column index out of range.");
}
Float3 Float4x4::GetTranslation() const
{
	return Float3(data[12], data[13], data[14]);
}
Float3 Float4x4::GetScale() const
{
	return Float3(GetColumn(0).Length(), GetColumn(1).Length(), GetColumn(2).Length());
}
Float3x3 Float4x4::GetRotation3x3() const
{
	return GetRotation3x3(GetScale());
}
Float4x4 Float4x4::GetRotation4x4() const
{
	return GetRotation4x4(GetScale());
}
Float3x3 Float4x4::GetRotation3x3(float scale) const
{
	return GetRotation3x3(Float3(scale));
}
Float4x4 Float4x4::GetRotation4x4(float scale) const
{
	return GetRotation4x4(Float3(scale));
}
Float3x3 Float4x4::GetRotation3x3(Float3 scale) const
{
	Float3 column0 = Float3(data[0] / scale.x, data[1] / scale.y, data[ 2] / scale.z).Normalize();
	Float3 column1 = Float3(data[4] / scale.x, data[5] / scale.y, data[ 6] / scale.z).Normalize();
	Float3 column2 = Float3(data[8] / scale.x, data[9] / scale.y, data[10] / scale.z).Normalize();
	return Float3x3::Columns
	(column0.x, column0.y, column0.z,
	 column1.x, column1.y, column1.z,
	 column2.x, column2.y, column2.z);
}
Float4x4 Float4x4::GetRotation4x4(Float3 scale) const
{
	return Float4x4(GetRotation3x3(scale));
}



// Assignment:
Float4x4& Float4x4::operator=(const Float4x4& other)
{
	if (this != &other)
	{
		for (uint32_t i = 0; i < 16; i++)
			data[i] = other.data[i];
	}
	return *this;
}
Float4x4& Float4x4::operator=(Float4x4&& other) noexcept
{
	if (this != &other)
	{
		for (uint32_t i = 0; i < 16; i++)
			data[i] = other.data[i];
	}
	return *this;
}



// Addition:
Float4x4 Float4x4::operator+(const Float4x4& other) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result.data[i] = data[i] + other.data[i];
	return result;
}
Float4x4& Float4x4::operator+=(const Float4x4& other)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] += other.data[i];
	return *this;
}



// Substraction:
Float4x4 Float4x4::operator-(const Float4x4& other) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result.data[i] = data[i] - other.data[i];
	return result;
}
Float4x4& Float4x4::operator-=(const Float4x4& other)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] -= other.data[i];
	return *this;
}
Float4x4 Float4x4::operator-() const
{
	return Float4x4
	(-data[0], -data[1], -data[2], -data[3],
	 -data[4], -data[5], -data[6], -data[7],
	 -data[8], -data[9], -data[10], -data[11],
	 -data[12], -data[13], -data[14], -data[15]);
}



// Multiplication:
Float4x4 Float4x4::operator*(const Float4x4& other) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 4; i++)
		for (uint32_t j = 0; j < 4; j++)
			for (uint32_t k = 0; k < 4; k++)
				result[{i, j}] += (*this)[{i, k}] * other[{k, j}];
	return result;
}
Float4x4& Float4x4::operator*=(const Float4x4& other)
{
	Float4x4 result;
	for (uint32_t i = 0; i < 4; i++)
		for (uint32_t j = 0; j < 4; j++)
			for (uint32_t k = 0; k < 4; k++)
				result[{i, j}] += (*this)[{i, k}] * other[{k, j}];
	*this = result;
	return *this;
}
Float4x4 Float4x4::operator*(float scalar) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result.data[i] = data[i] * scalar;
	return result;
}
Float4x4& Float4x4::operator*=(float scalar)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] *= scalar;
	return *this;
}
Float4 Float4x4::operator*(const Float4& vector) const
{
	return Float4
	(data[0] * vector.x + data[4] * vector.y + data[8] * vector.z + data[12] * vector.w,
	 data[1] * vector.x + data[5] * vector.y + data[9] * vector.z + data[13] * vector.w,
	 data[2] * vector.x + data[6] * vector.y + data[10] * vector.z + data[14] * vector.w,
	 data[3] * vector.x + data[7] * vector.y + data[11] * vector.z + data[15] * vector.w);
}



// Division:
Float4x4 Float4x4::operator/(float scalar) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result.data[i] = data[i] / scalar;
	return result;
}
Float4x4& Float4x4::operator/=(float scalar)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] /= scalar;
	return *this;
}



// Comparison:
bool Float4x4::IsEpsilonEqual(const Float4x4& other) const
{
	for (uint32_t i = 0; i < 16; i++)
		if (std::fabs(data[i] - other.data[i]) > epsilon)
			return false;
	return true;
}
bool Float4x4::operator==(const Float4x4& other) const
{
	for (uint32_t i = 0; i < 16; i++)
		if (data[i] != other.data[i])
			return false;
	return true;
}
bool Float4x4::operator!=(const Float4x4& other) const
{
	return !(*this == other);
}



// Friend functions:
Float4x4 operator*(float a, const Float4x4& b)
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result.data[i] = a * b.data[i];
	return result;
}



// Logging:
std::string Float4x4::ToString() const
{
	std::ostringstream oss;
	oss << "("   << data[0] << ", " << data[4] << ", " << data[ 8] << ", " << data[12];
	oss << " | " << data[1] << ", " << data[5] << ", " << data[ 9] << ", " << data[13];
	oss << " | " << data[2] << ", " << data[6] << ", " << data[10] << ", " << data[14];
	oss << " | " << data[3] << ", " << data[7] << ", " << data[11] << ", " << data[15] << ")";
	return oss.str();
}
std::string Float4x4::ToStringMatrixForm() const
{
	std::ostringstream oss;
	oss << "\n";
	oss << "(" << data[0] << ", " << data[4] << ", " << data[ 8] << ", " << data[12] << ")\n";
	oss << "(" << data[1] << ", " << data[5] << ", " << data[ 9] << ", " << data[13] << ")\n";
	oss << "(" << data[2] << ", " << data[6] << ", " << data[10] << ", " << data[14] << ")\n";
	oss << "(" << data[3] << ", " << data[7] << ", " << data[11] << ", " << data[15] << ")";
	return oss.str();
}
std::ostream& operator<<(std::ostream& os, const Float4x4& value)
{
	os << value.ToString();
	return os;
}



// Static members:
Float4x4 Float4x4::zero = Float4x4(0.0f);
Float4x4 Float4x4::identity = Float4x4
(1.0f, 0.0f, 0.0f, 0.0f,
 0.0f, 1.0f, 0.0f, 0.0f,
 0.0f, 0.0f, 1.0f, 0.0f,
 0.0f, 0.0f, 0.0f, 1.0f);