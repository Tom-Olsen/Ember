#include "float4x4.h"
#include "float4.h"
#include "float3x3.h"
#include "logger.h"
#include "uint3.h"
#include <stdexcept>
#include <sstream>



// Constructors:
Float4x4::Float4x4
(float xx, float xy, float xz, float xw,	// column 0
 float yx, float yy, float yz, float yw,	// column 1
 float zx, float zy, float zz, float zw,	// column 2
 float wx, float wy, float wz, float ww)	// column 3
{
	data[0] = xx; data[4] = yx; data[ 8] = zx; data[12] = wx;
	data[1] = xy; data[5] = yy; data[ 9] = zy; data[13] = wy;
	data[2] = xz; data[6] = yz; data[10] = zz; data[14] = wz;
	data[3] = xw; data[7] = yw; data[11] = zw; data[15] = ww;
}
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
Float4x4::Float4x4(const float* const array)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] = array[i];
}
Float4x4::Float4x4(const Float3x3& other)
{
	data[0] = other[0]; data[4] = other[3]; data[ 8] = other[6]; data[12] = 0.0f;
	data[1] = other[1]; data[5] = other[4]; data[ 9] = other[7]; data[13] = 0.0f;
	data[2] = other[2]; data[6] = other[5]; data[10] = other[8]; data[14] = 0.0f;
	data[3] =     0.0f; data[7] =     0.0f; data[11] =     0.0f; data[15] = 1.0f;
}
Float4x4::Float4x4(const Float4x4& other)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] = other[i];
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
	return IsEpsilonEqual(Float4x4::zero);
}



// Static math operations:
Float4x4 Float4x4::RotateX(float angle)
{
	float c = mathf::Cos(angle);
	float s = mathf::Sin(angle);
	return Float4x4::Rows
	(1.0f, 0.0f, 0.0f, 0.0f,
	 0.0f,    c,   -s, 0.0f,
	 0.0f,    s,    c, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::RotateY(float angle)
{
	float c = mathf::Cos(angle);
	float s = mathf::Sin(angle);
	return Float4x4::Rows
	(   c, 0.0f,    s, 0.0f,
	 0.0f, 1.0f, 0.0f, 0.0f,
	   -s, 0.0f,    c, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::RotateZ(float angle)
{
	float c = mathf::Cos(angle);
	float s = mathf::Sin(angle);
	return Float4x4::Rows
	(   c,   -s, 0.0f, 0.0f,
	    s,    c, 0.0f, 0.0f,
	 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::Rotate(const Float3& axis, float angle)
{
	float c = mathf::Cos(angle);
	float s = mathf::Sin(angle);
	float t = 1.0f - c;
	Float3 normalizedAxis = axis.Normalize();
	float x = normalizedAxis.x;
	float y = normalizedAxis.y;
	float z = normalizedAxis.z;
	return Float4x4::Rows
	(    x * x * t + c, x * y * t - z * s, x * z * t + y * s, 0.0f,
	 y * x * t + z * s,     y * y * t + c, y * z * t - x * s, 0.0f,
	 z * x * t - y * s, z * y * t + x * s,     z * z * t + c, 0.0f,
	              0.0f,              0.0f,              0.0f, 1.0f);
}
Float4x4 Float4x4::Rotate(const Float3& eulerAngles, const Uint3& rotationOrder, CoordinateSystem rotationSystem)
{
	Float4x4 rot[3] = { RotateX(eulerAngles.x), RotateY(eulerAngles.y), RotateZ(eulerAngles.z) };
	if (rotationSystem == CoordinateSystem::local)
		return rot[rotationOrder.x] * rot[rotationOrder.y] * rot[rotationOrder.z];
	else if (rotationSystem == CoordinateSystem::world)
		return rot[rotationOrder.z] * rot[rotationOrder.y] * rot[rotationOrder.x];
	return identity;
}
Float4x4 Float4x4::RotateFromTo(const Float3& from, const Float3& to)
{
	Float3 f = from.Normalize();
	Float3 t = to.Normalize();
	if (f.IsEpsilonEqual(t))
		return Float4x4::identity;
	if (f.IsEpsilonEqual(-t))
		return Float4x4::Rotate(geometry3d::GetOrhtogonalVector(f), mathf::pi);
	Float3 axis = Float3::Cross(from, to); // normalization not needed, as Rotate(...) will normalize it
	float angle = Float3::Angle(from, to);
	return Rotate(axis, angle);
}
Float4x4 Float4x4::RotateThreeLeg(const Float3& direction0Old, const Float3& direction0New, const Float3& direction1Old, const Float3& direction1New)
{
	// Rotate direction0Old to direction0New:
	Float3 axis = Float3::Cross(direction0Old, direction0New);
	float angle0 = Float3::Angle(direction0Old, direction0New);
	Float3x3 rot0 = Float3x3::Rotate(axis, angle0);

	// Compute missalignment angle between direction1New and direction1Old rotated by rot0:
	Float3 otherOldRotated = rot0 * direction1Old;
	Float3 planeNormal = Float3::Cross(direction1New, direction0New).Normalize();
	Float3 projection = geometry3d::PointToPlaneProjection(otherOldRotated, Float3::zero, planeNormal);
	float sign = mathf::Sign(Float3::Dot(Float3::Cross(otherOldRotated, projection), direction0New));
	float angle1 = sign * Float3::Angle(otherOldRotated, projection);
	if (Float3::Dot(direction1New, otherOldRotated) < 0)
		angle1 += mathf::pi;

	// Rotate by angle around direction0New:
	Float3x3 rot1 = Float3x3::Rotate(direction0New, angle1);

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
Float4x4 Float4x4::TRS(const Float3& position, const Float3x3& rotationMatrix, const Float3& scale)
{
	Float4x4 T = Translate(position);
	Float4x4 S = Scale(scale);
	return T * Float4x4(rotationMatrix) * S;
}
Float4x4 Float4x4::TRS(const Float3& position, const Float4x4& rotationMatrix, const Float3& scale)
{
	Float4x4 T = Translate(position);
	Float4x4 S = Scale(scale);
	return T * rotationMatrix * S;
}
Float4x4 Float4x4::Perspective(float fov, float aspectRatio, float nearClip, float farClip)
{
	//// OpenGL:
	//float tanHalfFov = mathf::Tan(0.5f * fov);
	//float xx = 1.0f / (aspectRatio * tanHalfFov);
	//float yy = -1.0f / tanHalfFov;
	//float zz = (nearClip + farClip) / (nearClip - farClip);
	//float zw = 2.0f * nearClip * farClip / (nearClip - farClip);
	//
	//return Float4x4::Rows
	//(xx,   0.0f,  0.0f, 0.0f,
	// 0.0f,   yy,  0.0f, 0.0f,
	// 0.0f, 0.0f,    zz,   zw,
	// 0.0f, 0.0f, -1.0f, 0.0f);

	// Vulkan:
	float tanHalfFov = mathf::Tan(0.5f * fov);
	float xx = 1.0f / (aspectRatio * tanHalfFov);
	float yy = -1.0f / tanHalfFov;
	float zz = -farClip / (farClip - nearClip);
	float zw = -farClip * nearClip / (farClip - nearClip);

	return Float4x4::Rows
	(xx,   0.0f, 0.0f, 0.0f,
	 0.0f,   yy, 0.0f, 0.0f,
	 0.0f, 0.0f,   zz,   zw,
	 0.0f, 0.0f, -1.0f, 0.0f);
}
Float4x4 Float4x4::Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip)
{
	//// OpenGL:
	//float xx = 2.0f / (right - left);
	//float yy = 2.0f / (top - bottom);
	//float zz = -2.0f / (farClip - nearClip);
	//float xw = -(right + left) / (right - left);
	//float yw = -(top + bottom) / (top - bottom);
	//float zw = -(farClip + nearClip) / (farClip - nearClip);
	//
	//return Float4x4::Rows
	//(xx, 0.0f,   0.0f,   xw,
	// 0.0f, yy,   0.0f,   yw,
	// 0.0f, 0.0f,   zz,   zw,
	// 0.0f, 0.0f, 0.0f, 1.0f);

	// Vulkan:
	float xx = 2.0f / (right - left);
	float yy = 2.0f / (bottom - top);
	float zz = 1.0f / (nearClip - farClip);
	float xw = -(right + left) / (right - left);
	float yw = -(bottom + top) / (bottom - top);
	float zw = nearClip / (nearClip - farClip);

	return Float4x4::Rows
	(  xx, 0.0f, 0.0f,   xw,
	 0.0f,   yy, 0.0f,   yw,
	 0.0f, 0.0f,   zz,   zw,
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
			data[i] = other[i];
	}
	return *this;
}
Float4x4& Float4x4::operator=(Float4x4&& other) noexcept
{
	if (this != &other)
	{
		for (uint32_t i = 0; i < 16; i++)
			data[i] = other[i];
	}
	return *this;
}



// Addition:
Float4x4 Float4x4::operator+(const Float4x4& other) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result[i] = data[i] + other[i];
	return result;
}
Float4x4& Float4x4::operator+=(const Float4x4& other)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] += other[i];
	return *this;
}



// Substraction:
Float4x4 Float4x4::operator-(const Float4x4& other) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result[i] = data[i] - other[i];
	return result;
}
Float4x4& Float4x4::operator-=(const Float4x4& other)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] -= other[i];
	return *this;
}
Float4x4 Float4x4::operator-() const
{
	return Float4x4
	(-data[ 0], -data[ 1], -data[ 2], -data[ 3],
	 -data[ 4], -data[ 5], -data[ 6], -data[ 7],
	 -data[ 8], -data[ 9], -data[10], -data[11],
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
	Float4x4 result = (*this) * other;
	*this = result;
	return *this;
}
Float4x4& Float4x4::operator*=(float scalar)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] *= scalar;
	return *this;
}



// Division:
Float4x4 Float4x4::operator/(float scalar) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result[i] = data[i] / scalar;
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
		if (std::fabs(data[i] - other[i]) > mathf::epsilon)
			return false;
	return true;
}
bool Float4x4::operator==(const Float4x4& other) const
{
	for (uint32_t i = 0; i < 16; i++)
		if (data[i] != other[i])
			return false;
	return true;
}
bool Float4x4::operator!=(const Float4x4& other) const
{
	return !(*this == other);
}



// Friend functions:
Float4x4 operator*(const Float4x4& a, float b)
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result[i] = a[i] * b;
	return result;
}
Float4x4 operator*(float a, const Float4x4& b)
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result[i] = a * b[i];
	return result;
}
Float4 operator*(const Float4x4& a, const Float4& b)
{
	return Float4
	(a[0] * b.x + a[4] * b.y + a[ 8] * b.z + a[12] * b.w,
	 a[1] * b.x + a[5] * b.y + a[ 9] * b.z + a[13] * b.w,
	 a[2] * b.x + a[6] * b.y + a[10] * b.z + a[14] * b.w,
	 a[3] * b.x + a[7] * b.y + a[11] * b.z + a[15] * b.w);
}
Float4 operator*(const Float4& a, const Float4x4& b)
{
	return Float4
	(a.x * b[ 0] + a.y * b[ 1] + a.z * b[ 2] + a.w * b[ 3],
	 a.x * b[ 4] + a.y * b[ 5] + a.z * b[ 6] + a.w * b[ 7],
	 a.x * b[ 8] + a.y * b[ 9] + a.z * b[10] + a.w * b[11],
	 a.x * b[12] + a.y * b[13] + a.z * b[14] + a.w * b[15]);
}



// Logging:
std::string Float4x4::ToString() const
{
	std::ostringstream oss;
	oss << "("   << data[ 0] << ", " << data[ 1] << ", " << data[ 2] << ", " << data[ 3];
	oss << " | " << data[ 4] << ", " << data[ 5] << ", " << data[ 6] << ", " << data[ 7];
	oss << " | " << data[ 8] << ", " << data[ 9] << ", " << data[10] << ", " << data[11];
	oss << " | " << data[12] << ", " << data[13] << ", " << data[14] << ", " << data[15] << ")";
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
// Numbers:
Float4x4 Float4x4::zero = Float4x4(0.0f);
Float4x4 Float4x4::identity = Float4x4
(1.0f, 0.0f, 0.0f, 0.0f,
 0.0f, 1.0f, 0.0f, 0.0f,
 0.0f, 0.0f, 1.0f, 0.0f,
 0.0f, 0.0f, 0.0f, 1.0f);
Float4x4 Float4x4::maxValue = Float4x4(mathf::maxValue);
Float4x4 Float4x4::minValue = Float4x4(mathf::minValue);

// Rotations:
Float4x4 Float4x4::rot45x = Float4x4::RotateX(mathf::pi4);
Float4x4 Float4x4::rot45y = Float4x4::RotateY(mathf::pi4);
Float4x4 Float4x4::rot45z = Float4x4::RotateZ(mathf::pi4);
Float4x4 Float4x4::rot90x = Float4x4::RotateX(mathf::pi2);
Float4x4 Float4x4::rot90y = Float4x4::RotateY(mathf::pi2);
Float4x4 Float4x4::rot90z = Float4x4::RotateZ(mathf::pi2);
Float4x4 Float4x4::rot135x = Float4x4::RotateX(3.0f * mathf::pi4);
Float4x4 Float4x4::rot135y = Float4x4::RotateY(3.0f * mathf::pi4);
Float4x4 Float4x4::rot135z = Float4x4::RotateZ(3.0f * mathf::pi4);
Float4x4 Float4x4::rot180x = Float4x4::RotateX(mathf::pi);
Float4x4 Float4x4::rot180y = Float4x4::RotateY(mathf::pi);
Float4x4 Float4x4::rot180z = Float4x4::RotateZ(mathf::pi);
Float4x4 Float4x4::rot225x = Float4x4::RotateX(5.0f * mathf::pi4);
Float4x4 Float4x4::rot225y = Float4x4::RotateY(5.0f * mathf::pi4);
Float4x4 Float4x4::rot225z = Float4x4::RotateZ(5.0f * mathf::pi4);
Float4x4 Float4x4::rot270x = Float4x4::RotateX(-mathf::pi2);
Float4x4 Float4x4::rot270y = Float4x4::RotateY(-mathf::pi2);
Float4x4 Float4x4::rot270z = Float4x4::RotateZ(-mathf::pi2);
Float4x4 Float4x4::rot315x = Float4x4::RotateX(-mathf::pi4);
Float4x4 Float4x4::rot315y = Float4x4::RotateY(-mathf::pi4);
Float4x4 Float4x4::rot315z = Float4x4::RotateZ(-mathf::pi4);