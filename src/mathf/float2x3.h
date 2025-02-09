#ifndef __INCLUDE_GUARD_float2x3_h__
#define __INCLUDE_GUARD_float2x3_h__
#include "mathf.h"
#include <string>



namespace emberEngine
{
	// Forward declerations:
	struct Float2;
	struct Float3;
	struct Float2x2;
	struct Float3x2;
	struct Float3x3;



	struct Float2x3
	{
		// 2 rows 3 columns matrix with column major order:
		// xx yx zx = 0  2  4 = [0,0] [0,1] [0,2]
		// xy yy zy   1  3  5   [1,0] [1,1] [1,2]

	private:
		Float2x3
		(float xx, float xy,	// column 0
			float yx, float yy,	// column 1
			float zx, float zy);	// column 2

	public:
		// Members:
		float data[6];

		// Constructors:
		Float2x3();
		Float2x3(float value);
		Float2x3(const float* const array);
		Float2x3(const Float2x3& other);

		// Static constructors:
		static Float2x3 Rows(const Float3& row0, const Float3& row1);
		static Float2x3 Rows
		(float row0x, float row0y, float row0z,
			float row1x, float row1y, float row1z);
		static Float2x3 Columns(const Float2& column0, const Float2& column1, const Float2& column2);
		static Float2x3 Columns
		(float column0x, float column0y,
			float column1x, float column1y,
			float column2x, float column2y);

		// Math operations:
		Float3x2 Transpose() const;
		Float3x2 RightInverse() const;
		bool IsEpsilonZero() const;

		// Access:
		float& operator[](int index);
		float operator[](int index) const;
		float& operator[](const Index2& index);
		float operator[](const Index2& index) const;
		Float3 GetRow(int index) const;
		Float2 GetColumn(int index) const;

		// Assignment:
		Float2x3& operator=(const Float2x3& other);
		Float2x3& operator=(Float2x3&& other) noexcept;

		// Addition:
		Float2x3 operator+(const Float2x3& other) const;
		Float2x3& operator+=(const Float2x3& other);

		// Substraction:
		Float2x3 operator-(const Float2x3& other) const;
		Float2x3& operator-=(const Float2x3& other);
		Float2x3 operator-() const;

		// Multiplication:
		Float2x3& operator*=(float scalar);

		// Division:
		Float2x3 operator/(float scalar) const;
		Float2x3& operator/=(float scalar);

		// Comparison:
		bool IsEpsilonEqual(const Float2x3& other) const;
		bool operator==(const Float2x3& other) const;
		bool operator!=(const Float2x3& other) const;

		// Friend functions:
		friend Float2x3 operator*(const Float2x3& a, float b);
		friend Float2x3 operator*(float a, const Float2x3& b);
		friend Float2 operator*(const Float2x3& a, const Float3& b);
		friend Float3 operator*(const Float2& a, const Float2x3& b);
		friend Float2x2 operator*(const Float2x3& a, const Float3x2& b);
		friend Float2x3 operator*(const Float2x3& a, const Float3x3& b);
		friend Float2x3 operator*(const Float2x2& a, const Float2x3& b);
		friend Float3x3 operator*(const Float3x2& a, const Float2x3& b);

		// Logging:
		std::string ToString() const;
		std::string ToStringMatrixForm() const;
		friend std::ostream& operator<<(std::ostream& os, const Float3x3& value);

		// Static members:
		static Float2x3 zero;		// zero matrix.
		static Float2x3 maxValue;	// all entries are mathf::maxValue.
		static Float2x3 minValue;	// all entries are mathf::minValue.
	};
}



#endif // __INCLUDE_GUARD_float2x3_h__