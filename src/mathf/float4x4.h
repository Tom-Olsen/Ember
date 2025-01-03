#ifndef __INCLUDE_GUARD_float4x4_h__
#define __INCLUDE_GUARD_float4x4_h__
#include "mathf.h"
#include <string>



struct Float3;
struct Float4;
struct Float3x3;
struct Index2;
struct Uint3;



struct Float4x4
{
	// 4 rows 4 columns matrix with column major order:
	// xx yx zx wx   0  4   8 12   [0,0] [0,1] [0,2] [0,3]
	// xy yy zy wy = 1  5   9 13 = [1,0] [1,1] [1,2] [1,3]
	// xz yz zz wz   2  6  10 14   [2,0] [2,1] [2,2] [2,3]
	// xw yw zw ww   3  7  11 15   [3,0] [3,1] [3,2] [3,3]

private:
	Float4x4
	(float xx, float xy, float xz, float xw,	// column 0
	 float yx, float yy, float yz, float yw,	// column 1
	 float zx, float zy, float zz, float zw,	// column 2
	 float wx, float wy, float wz, float ww);	// column 3

public:
	// Members:
	float data[16];

	// Constructors:
	Float4x4();
	Float4x4(float value);
	Float4x4(const float* const array);
	explicit Float4x4(const Float3x3& other);
	Float4x4(const Float4x4& other);

	// Static constructors:
	static Float4x4 Rows(const Float4& row0, const Float4& row1, const Float4& row2, const Float4& row3);
	static Float4x4 Rows
	(float row0x, float row0y, float row0z, float row0w,
	 float row1x, float row1y, float row1z, float row1w,
	 float row2x, float row2y, float row2z, float row2w,
	 float row3x, float row3y, float row3z, float row3w);
	static Float4x4 Columns(const Float4& column0, const Float4& column1, const Float4& column2, const Float4& column3);
	static Float4x4 Columns
	(float column0x, float column0y, float column0z, float column0w,
	 float column1x, float column1y, float column1z, float column1w,
	 float column2x, float column2y, float column2z, float column2w,
	 float column3x, float column3y, float column3z, float column3w);

	// Math operations:
	Float4x4 Transpose();
	float Determinant() const;
	Float4x4 Inverse() const;
	Float4x4 Inverse(float det) const;
	bool IsEpsilonZero() const;

	// Static math operations:
	static Float4x4 RotateX(float angle);
	static Float4x4 RotateY(float angle);
	static Float4x4 RotateZ(float angle);
	static Float4x4 Rotate(const Float3& axis, float angle);
	static Float4x4 Rotate(const Float3& eulerAngles, const Uint3& rotationOrder = Uint3(1, 0, 2), CoordinateSystem rotationSystem = CoordinateSystem::local);
	static Float4x4 RotateFromTo(const Float3& from, const Float3& to);
	static Float4x4 RotateThreeLeg(const Float3& forwardOld, const Float3& forwardNew, const Float3& otherOld, const Float3& otherNew);
	static Float4x4 Translate(const Float3& translation);
	static Float4x4 Scale(const Float3& scale);
	static Float4x4 Scale(float scale);
	static Float4x4 TRS(const Float3& position, const Float3x3& rotationMatrix, const Float3& scale);
	static Float4x4 TRS(const Float3& position, const Float4x4& rotationMatrix, const Float3& scale);
	static Float4x4 Perspective(float fov, float aspectRatio, float nearClip, float farClip);
	static Float4x4 Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip);

	// Access:
	float& operator[](int index);
	float operator[](int index) const;
	float& operator[](const Index2& index);
	float operator[](const Index2& index) const;
	Float4 GetRow(int index) const;
	Float4 GetColumn(int index) const;
	Float3 GetTranslation() const;
	Float3 GetScale() const;
	Float3x3 GetRotation3x3() const;
	Float4x4 GetRotation4x4() const;
	Float3x3 GetRotation3x3(Float3 scale) const;
	Float4x4 GetRotation4x4(Float3 scale) const;
	Float3x3 GetRotation3x3(float scale) const;
	Float4x4 GetRotation4x4(float scale) const;

	// Assignment:
	Float4x4& operator=(const Float4x4& other);
	Float4x4& operator=(Float4x4&& other) noexcept;

	// Addition:
	Float4x4 operator+(const Float4x4& other) const;
	Float4x4& operator+=(const Float4x4& other);

	// Substraction:
	Float4x4 operator-(const Float4x4& other) const;
	Float4x4& operator-=(const Float4x4& other);
	Float4x4 operator-() const;

	// Multiplication:
	Float4x4 operator*(const Float4x4& other) const;
	Float4x4& operator*=(const Float4x4& other);
	Float4x4& operator*=(float scalar);

	// Division:
	Float4x4 operator/(float scalar) const;
	Float4x4& operator/=(float scalar);

	// Comparison:
	bool IsEpsilonEqual(const Float4x4& other) const;
	bool operator==(const Float4x4& other) const;
	bool operator!=(const Float4x4& other) const;

	// Friend functions:
	friend Float4x4 operator*(const Float4x4& a, float b);
	friend Float4x4 operator*(float a, const Float4x4& b);
	friend Float4 operator*(const Float4x4& a, const Float4& b);
	friend Float4 operator*(const Float4& a, const Float4x4& b);

	// Logging:
	std::string ToString() const;
	std::string ToStringMatrixForm() const;
	friend std::ostream& operator<<(std::ostream& os, const Float4x4& value);

	// Static members:
	static Float4x4 zero;		// zero matrix.
	static Float4x4 identity;	// identity matrix.
};



#endif // __INCLUDE_GUARD_float4x4_h__