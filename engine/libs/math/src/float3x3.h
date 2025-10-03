#pragma once
#include "mathConstants.h"
#include "uint3.h"	// needed for default value: rotationOrder = Uint3(1, 0, 2)
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct Float2;
	struct Float3;
	struct Float4x4;



	struct Float3x3
	{
		// 3 rows 3 columns matrix with column major order:
		// xx yx zx   0  3  6   [0,0] [0,1] [0,2]
		// xy yy zy = 1  4  7 = [1,0] [1,1] [1,2]
		// xz yz zz   2  5  8   [2,0] [2,1] [2,2]

	private:
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
		Float3x3(const float* const array);
		Float3x3(const Float3x3& other);
		explicit Float3x3(const Float4x4& other);

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
		static Float3x3 RotateX(float angle);
		static Float3x3 RotateY(float angle);
		static Float3x3 RotateZ(float angle);
		static Float3x3 Rotate(const Float3& axis, float angle);
		static Float3x3 Rotate(const Float3& angles, const Uint3& rotationOrder = Uint3(1, 0, 2), CoordinateSystem rotationSystem = CoordinateSystem::local);
		static Float3x3 RotateFromTo(const Float3& from, const Float3& to);
		static Float3x3 RotateThreeLeg(const Float3& direction0Old, const Float3& direction0New, const Float3& direction1Old, const Float3& direction1New);

		// Access:
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
		Float3x3& operator*=(float scalar);

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
		friend Float3 operator*(const Float3x3& a, const Float3& b);
		friend Float3 operator*(const Float3& a, const Float3x3& b);

		// Logging:
		std::string ToString() const;
		std::string ToStringMatrixForm() const;
		friend std::ostream& operator<<(std::ostream& os, const Float3x3& value);

		// Static members:
		static Float3x3 zero;		// zero matrix.
		static Float3x3 identity;	// identity matrix.
		static Float3x3 maxValue;	// all entries are math::maxValue.
		static Float3x3 minValue;	// all entries are math::minValue.

		// Rotations:
		static Float3x3 rot45x;		// 45 degrees rotation around x-axis.
		static Float3x3 rot45y;		// 45 degrees rotation around y-axis.
		static Float3x3 rot45z;		// 45 degrees rotation around z-axis.
		static Float3x3 rot90x;		// 90 degrees rotation around x-axis.
		static Float3x3 rot90y;		// 90 degrees rotation around y-axis.
		static Float3x3 rot90z;		// 90 degrees rotation around z-axis.
		static Float3x3 rot135x;	// 135 degrees rotation around x-axis.
		static Float3x3 rot135y;	// 135 degrees rotation around y-axis.
		static Float3x3 rot135z;	// 135 degrees rotation around z-axis.
		static Float3x3 rot180x;	// 180 degrees rotation around x-axis.
		static Float3x3 rot180y;	// 180 degrees rotation around y-axis.
		static Float3x3 rot180z;	// 180 degrees rotation around z-axis.
		static Float3x3 rot225x;	// 225 degrees rotation around x-axis.
		static Float3x3 rot225y;	// 225 degrees rotation around y-axis.
		static Float3x3 rot225z;	// 225 degrees rotation around z-axis.
		static Float3x3 rot270x;	// 270 degrees rotation around x-axis.
		static Float3x3 rot270y;	// 270 degrees rotation around y-axis.
		static Float3x3 rot270z;	// 270 degrees rotation around z-axis.
		static Float3x3 rot315x;	// 315 degrees rotation around x-axis.
		static Float3x3 rot315y;	// 315 degrees rotation around y-axis.
		static Float3x3 rot315z;	// 315 degrees rotation around z-axis.
	};
}