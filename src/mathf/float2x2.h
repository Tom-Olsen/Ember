#ifndef __INCLUDE_GUARD_float2x2_h__
#define __INCLUDE_GUARD_float2x2_h__
#include "mathf.h"
#include <string>



struct Float2;
struct Index2;



struct Float2x2
{
	// 2 rows 2 columns matrix with column major order:
	// xx yx = 0  2  = [0,0] [0,1]
	// xy yy   1  3    [1,0] [1,1]

private:
	Float2x2
	(float xx, float xy,	// column 0
	 float yx, float yy);	// column 1

public:
	// Members:
	float data[4];

	// Constructors:
	Float2x2();
	Float2x2(float value);
	Float2x2(const float* const array);
	Float2x2(const Float2x2& other);

	// Static constructors:
	static Float2x2 Rows(const Float2& row0, const Float2& row1);
	static Float2x2 Rows
	(float row0x, float row0y,
	 float row1x, float row1y);
	static Float2x2 Columns(const Float2& column0, const Float2& column1);
	static Float2x2 Columns
	(float column0x, float column0y,
	 float column1x, float column1y);

	// Math operations:
	Float2x2 Transpose() const;
	float Determinant() const;
	Float2x2 Inverse() const;
	Float2x2 Inverse(float det) const;
	bool IsEpsilonZero() const;

	// Static math operations:
	static Float2x2 Rotate(float radians);

	// Access:
	float& operator[](int index);
	float operator[](int index) const;
	float& operator[](const Index2& index);
	float operator[](const Index2& index) const;
	Float2 GetRow(int index) const;
	Float2 GetColumn(int index) const;

	// Assignment:
	Float2x2& operator=(const Float2x2& other);
	Float2x2& operator=(Float2x2&& other) noexcept;

	// Addition:
	Float2x2 operator+(const Float2x2& other) const;
	Float2x2& operator+=(const Float2x2& other);

	// Substraction:
	Float2x2 operator-(const Float2x2& other) const;
	Float2x2& operator-=(const Float2x2& other);
	Float2x2 operator-() const;

	// Multiplication:
	Float2x2 operator*(const Float2x2& other) const;
	Float2x2& operator*=(const Float2x2& other);
	Float2x2& operator*=(float scalar);

	// Division:
	Float2x2 operator/(float scalar) const;
	Float2x2& operator/=(float scalar);

	// Comparison:
	bool IsEpsilonEqual(const Float2x2& other) const;
	bool operator==(const Float2x2& other) const;
	bool operator!=(const Float2x2& other) const;

	// Friend functions:
	friend Float2x2 operator*(const Float2x2& a, float b);
	friend Float2x2 operator*(float a, const Float2x2& b);
	friend Float2 operator*(const Float2x2& a, const Float2& b);
	friend Float2 operator*(const Float2& a, const Float2x2& b);

	// Logging:
	std::string ToString() const;
	std::string ToStringMatrixForm() const;
	friend std::ostream& operator<<(std::ostream& os, const Float2x2& value);

	// Static members:
	static Float2x2 zero;		// zero matrix.
	static Float2x2 identity;	// identity matrix.
	static Float2x2 maxValue;	// all entries are mathf::maxValue.
	static Float2x2 minValue;	// all entries are mathf::minValue.
};



#endif // __INCLUDE_GUARD_float2x2_h__