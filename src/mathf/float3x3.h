#ifndef __INCLUDE_GUARD_float3x3_h__
#define __INCLUDE_GUARD_float3x3_h__
#include "mathf.h"
#include <string>



struct Float3;
struct Float4x4;
struct Index2;
struct Uint3;



struct Float3x3
{
	// Computer science convention (transposed to proper math convention):
	// xx yx zx   0  3  6   [0,0] [1,0] [2,0]
	// xy yy zy = 1  4  7 = [0,1] [1,1] [2,1]
	// xz yz zz   2  5  8   [0,2] [1,2] [2,2]

private:
	constexpr static float epsilon = 1e-8f;
	Float3x3
	(float xx, float xy, float xz,	// column 0
	 float yx, float yy, float yz,	// column 1
	 float zx, float zy, float zz);	// column 2

public:
	// Members:
	float data[9];

	// Constructors:
	Float3x3();
	Float3x3(float value);
	Float3x3(const float* array); // Initialize from a 1D array
	Float3x3(const Float3x3& other);
	Float3x3(const Float4x4& other);

	// Static constructors:
	static Float3x3 Rows(const Float3& row0, const Float3& row1, const Float3& row2);
	static Float3x3 Rows
	(float row0x, float row0y, float row0z,
	 float row1x, float row1y, float row1z,
	 float row2x, float row2y, float row2z);
	static Float3x3 Columns(const Float3& column0, const Float3& column1, const Float3& column2);
	static Float3x3 Columns
	(float column0x, float column0y, float column0z,
	 float column1x, float column1y, float column1z,
	 float column2x, float column2y, float column2z);

	// Math operations:
	Float3x3 Transpose() const;
	float Determinant() const;
	Float3x3 Inverse() const;
	Float3x3 Inverse(float det) const;
	bool IsEpsilonZero() const;

	// Static math operations:
	static Float3x3 RotateX(float radians);
	static Float3x3 RotateY(float radians);
	static Float3x3 RotateZ(float radians);
	static Float3x3 Rotate(const Float3& axis, float radians);
	static Float3x3 Rotate(const Float3& eulerRadians, const Uint3& rotationOrder = Uint3(1, 0, 2), CoordinateSystem rotationSystem = CoordinateSystem::local);
	static Float3x3 RotateFromTo(const Float3& from, const Float3& to);
	static Float3x3 RotateThreeLeg(const Float3& forwardOld, const Float3& forwardNew, const Float3& upOld, const Float3& upNew);

	// Access:
	float& operator[](int index);
	float operator[](int index) const;
	float& operator[](const Index2& index);
	float operator[](const Index2& index) const;
	Float3 GetRow(int index) const;
	Float3 GetColumn(int index) const;

	// Assignment:
	Float3x3& operator=(const Float3x3& other);
	Float3x3& operator=(Float3x3&& other) noexcept;

	// Addition:
	Float3x3 operator+(const Float3x3& other) const;
	Float3x3& operator+=(const Float3x3& other);

	// Substraction:
	Float3x3 operator-(const Float3x3& other) const;
	Float3x3& operator-=(const Float3x3& other);
	Float3x3 operator-() const;

	// Multiplication:
	Float3x3 operator*(const Float3x3& other) const;
	Float3x3& operator*=(const Float3x3& other);
	Float3x3 operator*(float scalar) const;
	Float3x3& operator*=(float scalar);
	Float3 operator*(const Float3& vector) const;

	// Division:
	Float3x3 operator/(float scalar) const;
	Float3x3& operator/=(float scalar);

	// Comparison:
	bool IsEpsilonEqual(const Float3x3& other) const;
	bool operator==(const Float3x3& other) const;
	bool operator!=(const Float3x3& other) const;

	// Friend functions:
	friend Float3x3 operator*(const Float3x3& a, float b);
	friend Float3x3 operator*(float a, const Float3x3& b);
	friend Float3x3 operator/(const Float3x3& a, float b);

	// Logging:
	std::string ToString() const;
	std::string ToStringMatrixForm() const;
	friend std::ostream& operator<<(std::ostream& os, const Float3x3& value);

	// Static members:
	static Float3x3 zero;		// zero matrix.
	static Float3x3 identity;	// identity matrix.
};



#endif // __INCLUDE_GUARD_float3x3_h__