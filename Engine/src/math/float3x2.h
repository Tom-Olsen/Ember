#ifndef __INCLUDE_GUARD_float3x2_h__
#define __INCLUDE_GUARD_float3x2_h__
#include "mathConstants.h"
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct Float2;
	struct Float3;
	struct Float2x2;
	struct Float2x3;
	struct Float3x3;



	struct Float3x2
	{
		// 3 rows 2 columns matrix with column major order:
		// xx yx   0  3   [0,0] [0,1]
		// xy yy = 1  4 = [1,0] [1,1]
		// xz yz   2  5   [2,0] [2,1]

	private:
		Float3x2
		(float xx, float xy, float xz,	// column 0
		 float yx, float yy, float yz);	// column 1

	public:
		// Members:
		float data[6];

		// Constructors:
		Float3x2();
		Float3x2(float value);
		Float3x2(const float* const array);
		Float3x2(const Float3x2& other);

		// Static constructors:
		static Float3x2 Rows(const Float2& row0, const Float2& row1, const Float2& row2);
		static Float3x2 Rows
		(float row0x, float row0y,
		 float row1x, float row1y,
		 float row2x, float row2y);
		static Float3x2 Columns(const Float3& column0, const Float3& column1);
		static Float3x2 Columns
		(float column0x, float column0y, float column0z,
		 float column1x, float column1y, float column1z);

		// Math operations:
		Float2x3 Transpose() const;
		Float2x3 LeftInverse() const;
		bool IsEpsilonZero() const;

		// Access:
		float& operator[](const Index2& index);
		float operator[](const Index2& index) const;
		Float2 GetRow(int index) const;
		Float3 GetColumn(int index) const;

		// Assignment:
		Float3x2& operator=(const Float3x2& other);
		Float3x2& operator=(Float3x2&& other) noexcept;

		// Addition:
		Float3x2 operator+(const Float3x2& other) const;
		Float3x2& operator+=(const Float3x2& other);

		// Substraction:
		Float3x2 operator-(const Float3x2& other) const;
		Float3x2& operator-=(const Float3x2& other);
		Float3x2 operator-() const;

		// Multiplication:
		Float3x2& operator*=(float scalar);

		// Division:
		Float3x2 operator/(float scalar) const;
		Float3x2& operator/=(float scalar);

		// Comparison:
		bool IsEpsilonEqual(const Float3x2& other) const;
		bool operator==(const Float3x2& other) const;
		bool operator!=(const Float3x2& other) const;

		// Friend functions:
		friend Float3x2 operator*(const Float3x2& a, float b);
		friend Float3x2 operator*(float a, const Float3x2& b);
		friend Float3 operator*(const Float3x2& a, const Float2& b);
		friend Float2 operator*(const Float3& a, const Float3x2& b);
		friend Float3x2 operator*(const Float3x2& a, const Float2x2& b);
		friend Float3x2 operator*(const Float3x3& a, const Float3x2& b);

		// Logging:
		std::string ToString() const;
		std::string ToStringMatrixForm() const;
		friend std::ostream& operator<<(std::ostream& os, const Float3x3& value);

		// Static members:
		static Float3x2 zero;		// zero matrix.
		static Float3x2 maxValue;	// all entries are math::maxValue.
		static Float3x2 minValue;	// all entries are math::minValue.
	};
}



#endif // __INCLUDE_GUARD_float3x2_h__