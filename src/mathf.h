#pragma once
#ifndef __INCLUDE_GUARD_mathf_h__
#define __INCLUDE_GUARD_mathf_h__
#include <string>



namespace mathf
{
	class Float3;
	class Float4;
	class Float3x3;



	struct Index2
	{
		uint32_t i, j;
	};
	struct Index3
	{
		uint32_t i, j, k;
	};



	class Int2
	{
	public:
		int x, y;
		Int2(int xy);
		Int2(int x, int y);
		Int2(const Int2& xy);

		// Object math operations:

		// Static math operations:

		// Access:
		int& operator[](int index);
		int operator[](int index) const;

		// Assignment:
		Int2& operator=(const Int2& other);
		Int2& operator=(Int2&& other) noexcept;

		// Addition:
		Int2 operator+(const Int2& other) const;
		Int2& operator+=(const Int2& other);

		// Substraction:
		Int2 operator-(const Int2& other) const;
		Int2& operator-=(const Int2& other);

		// Multiplication:
		Int2 operator*(const Int2& other) const;
		Int2& operator*=(const Int2& other);
		Int2 operator*(int scalar) const;
		Int2& operator*=(int scalar);

		// Comparison:
		bool operator==(const Int2& other) const;
		bool operator!=(const Int2& other) const;

		// Friend functions:
		friend Int2 operator*(int a, const Int2& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Int2& value);

		// Static members:
		static Int2 zero;
		static Int2 one;
		static Int2 right;
		static Int2 left;
		static Int2 up;
		static Int2 down;
	};



	class Int3
	{
	public:
		int x, y, z;
		Int3(int xyz);
		Int3(int x, int y);
		Int3(int x, int y, int z);
		Int3(const Int2& xy);
		Int3(const Int2& xy, int z);
		Int3(const Int3& xyz);

		// Object math operations:

		// Static math operations:

		// Access:
		int& operator[](int index);
		int operator[](int index) const;

		// Assignment:
		Int3& operator=(const Int3& other);
		Int3& operator=(Int3&& other) noexcept;

		// Addition:
		Int3 operator+(const Int3& other) const;
		Int3& operator+=(const Int3& other);

		// Substraction:
		Int3 operator-(const Int3& other) const;
		Int3& operator-=(const Int3& other);

		// Multiplication:
		Int3 operator*(const Int3& other) const;
		Int3& operator*=(const Int3& other);
		Int3 operator*(int scalar) const;
		Int3& operator*=(int scalar);

		// Comparison:
		bool operator==(const Int3& other) const;
		bool operator!=(const Int3& other) const;

		// Friend functions:
		friend Int3 operator*(int a, const Int3& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Int3& value);

		// Static members:
		static Int3 zero;
		static Int3 one;
		static Int3 right;
		static Int3 left;
		static Int3 up;
		static Int3 down;
		static Int3 forward;
		static Int3 backward;
	};



	class Float2
	{
	private:
		constexpr static float epsilon = 1e-8f;
	public:
		float x, y;
		Float2(float xy);
		Float2(float x, float y);
		Float2(const Float2& xy);
		Float2(const Float3& xy);
		Float2(const Float4& xy);
		static Float2 Direction(float angle);

		// Object math operations:
		float Length2() const;
		float Length() const;
		float Angle() const;
		Float2 Normalize() const;
		Float2 Rotate(float angle) const;
		Float2 Rotate90() const;
		Float2 Rotate270() const;
		bool IsEpsilonZero() const;

		// Static math operations:
		static float Dot(const Float2& a, const Float2& b);
		static float Cross(const Float2& a, const Float2& b);
		static Float2 Project(const Float2& vec, const Float2& axes);
		static Float2 Reflect(const Float2& vec, const Float2& axes);
		static float Distance2(const Float2& a, const Float2& b);
		static float Distance(const Float2& a, const Float2& b);
		static float Angle(const Float2& a, const Float2& b);

		// Access:
		float& operator[](int index);
		float operator[](int index) const;

		// Assignment:
		Float2& operator=(const Float2& other);
		Float2& operator=(Float2&& other) noexcept;

		// Addition:
		Float2 operator+(const Float2& other) const;
		Float2& operator+=(const Float2& other);

		// Substraction:
		Float2 operator-(const Float2& other) const;
		Float2& operator-=(const Float2& other);

		// Multiplication:
		Float2 operator*(const Float2& other) const;
		Float2& operator*=(const Float2& other);
		Float2 operator*(float scalar) const;
		Float2& operator*=(float scalar);

		// Division:
		Float2 operator/(const Float2& other) const;
		Float2& operator/=(const Float2& other);
		Float2 operator/(float scalar) const;
		Float2& operator/=(float scalar);

		// Comparison:
		bool IsEpsilonEqual(const Float2& other) const;
		bool operator==(const Float2& other) const;
		bool operator!=(const Float2& other) const;

		// Friend functions:
		friend Float2 operator*(float a, const Float2& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Float2& value);

		// Static members:
		static Float2 zero;
		static Float2 one;
		static Float2 right;
		static Float2 left;
		static Float2 up;
		static Float2 down;
	};



	class Float3
	{
	private:
		constexpr static float epsilon = 1e-8f;
	public:
		float x, y, z;
		Float3(float xyz);
		Float3(float x, float y);
		Float3(float x, float y, float z);
		Float3(const Float2& xy);
		Float3(const Float2& xy, float z);
		Float3(const Float3& xyz);
		Float3(const Float4& xyz);
		static Float3 Direction(float theta, float phi);

		// Object math operations:
		float Length2() const;
		float Length() const;
		float Theta() const;
		float Phi() const;
		Float2 Angles() const;
		Float3 Normalize() const;
		Float3 Rotate(float theta, float phi) const;
		bool IsEpsilonZero() const;

		// Static math operations:
		static float Dot(const Float3& a, const Float3& b);
		static Float3 Cross(const Float3& a, const Float3& b);
		static Float3 Project(const Float3& vec, const Float3& planeNormal);
		static Float3 Reflect(const Float3& vec, const Float3& planeNormal);
		static float Distance2(const Float3& a, const Float3& b);
		static float Distance(const Float3& a, const Float3& b);
		static float Angle(const Float3& a, const Float3& b);

		// Access:
		float& operator[](int index);
		float operator[](int index) const;

		// Assignment:
		Float3& operator=(const Float3& other);
		Float3& operator=(Float3&& other) noexcept;

		// Addition:
		Float3 operator+(const Float3& other) const;
		Float3& operator+=(const Float3& other);

		// Substraction:
		Float3 operator-(const Float3& other) const;
		Float3& operator-=(const Float3& other);

		// Multiplication:
		Float3 operator*(const Float3& other) const;
		Float3& operator*=(const Float3& other);
		Float3 operator*(float scalar) const;
		Float3& operator*=(float scalar);

		// Division:
		Float3 operator/(const Float3& other) const;
		Float3& operator/=(const Float3& other);
		Float3 operator/(float scalar) const;
		Float3& operator/=(float scalar);

		// Comparison:
		bool IsEpsilonEqual(const Float3& other) const;
		bool operator==(const Float3& other) const;
		bool operator!=(const Float3& other) const;

		// Friend functions:
		friend Float3 operator*(float a, const Float3& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Float3& value);

		// Static members:
		static Float3 zero;
		static Float3 one;
		static Float3 right;
		static Float3 left;
		static Float3 up;
		static Float3 down;
		static Float3 forward;
		static Float3 backward;
	};



	class Float4
	{
	private:
		constexpr static float epsilon = 1e-8f;
	public:
		float x, y, z, w;
		Float4(float xyzw);
		Float4(float x, float y);
		Float4(float x, float y, float z);
		Float4(float x, float y, float z, float w);
		Float4(const Float2& xy);
		Float4(const Float2& xy, float z);
		Float4(const Float2& xy, float z, float w);
		Float4(const Float2& xy, Float2 zw);
		Float4(const Float3& xyz);
		Float4(const Float3& xyz, float w);
		Float4(const Float4& xyzw);
		static Float4 Direction(float theta, float phi);

		// Object math operations:
		float Length2() const;
		float Length() const;
		bool IsEpsilonZero() const;

		// Static math operations:

		// Access:
		float& operator[](int index);
		float operator[](int index) const;
		Float3 xyz() const;

		// Assignment:
		Float4& operator=(const Float4& other);
		Float4& operator=(Float4&& other) noexcept;

		// Addition:
		Float4 operator+(const Float4& other) const;
		Float4& operator+=(const Float4& other);

		// Substraction:
		Float4 operator-(const Float4& other) const;
		Float4& operator-=(const Float4& other);

		// Multiplication:
		Float4 operator*(const Float4& other) const;
		Float4& operator*=(const Float4& other);
		Float4 operator*(float scalar) const;
		Float4& operator*=(float scalar);

		// Division:
		Float4 operator/(const Float4& other) const;
		Float4& operator/=(const Float4& other);
		Float4 operator/(float scalar) const;
		Float4& operator/=(float scalar);

		// Comparison:
		bool IsEpsilonEqual(const Float4& other) const;
		bool operator==(const Float4& other) const;
		bool operator!=(const Float4& other) const;

		// Friend functions:
		friend Float4 operator*(float a, const Float4& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Float4& value);

		// Static members:
		static Float4 zero;
		static Float4 one;
		static Float4 right;
		static Float4 left;
		static Float4 up;
		static Float4 down;
		static Float4 forward;
		static Float4 backward;
		static Float4 in;
		static Float4 out;
	};



	class Float3x3
	{
	private:
		constexpr static float epsilon = 1e-8f;
		Float3x3
		(float xx, float xy, float xz,	// column 0
		 float yx, float yy, float yz,	// column 1
		 float zx, float zy, float zz);	// column 2

	public:
		float data[9];
		// Computer science convention (transposed to proper math convention):
		// xx yx zx   0  3  6   [0,0] [1,0] [2,0]
		// xy yy zy = 1  4  7 = [0,1] [1,1] [2,1]
		// xz yz zz   2  5  8   [0,2] [1,2] [2,2]
		Float3x3();
		Float3x3(float value);
		Float3x3(const float* array); // Initialize from a 1D array
		Float3x3(const Float3x3& other);

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

		// Object math operations:
		Float3x3 Transpose();
		float Determinant() const;
		Float3x3 Inverse() const;
		bool IsEpsilonZero() const;

		// Static math operations:
		static Float3x3 RotationX(float angle);
		static Float3x3 RotationY(float angle);
		static Float3x3 RotationZ(float angle);
		static Float3x3 Rotation(const Float3& axis, float angle);
		static Float3x3 RotationFromTo(const Float3& from, const Float3& to);

		// Access:
		float& operator[](const Index2& index);
		float operator[](const Index2& index) const;
		Float3 Row(int index) const;
		Float3 Column(int index) const;

		// Assignment:
		Float3x3& operator=(const Float3x3& other);
		Float3x3& operator=(Float3x3&& other) noexcept;

		// Addition:
		Float3x3 operator+(const Float3x3& other) const;
		Float3x3& operator+=(const Float3x3& other);

		// Substraction:
		Float3x3 operator-(const Float3x3& other) const;
		Float3x3& operator-=(const Float3x3& other);

		// Multiplication:
		Float3x3 operator*(const Float3x3& other) const;
		Float3x3& operator*=(const Float3x3& other);
		Float3x3 operator*(float scalar) const;
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
		friend Float3x3 operator/(const Float3x3& a, float b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Float3x3& value);

		// Static members:
		static Float3x3 zero;
		static Float3x3 identity;
	};



	class Float4x4
	{
	private:
		constexpr static float epsilon = 1e-8f;
		Float4x4
		(float xx, float xy, float xz, float xw,	// column 0
		 float yx, float yy, float yz, float yw,	// column 1
		 float zx, float zy, float zz, float zw,	// column 2
		 float wx, float wy, float wz, float ww);	// column 3

	public:
		float data[16];
		// Computer science convention (transposed to proper math convention):
		// xx yx zx wx   0  4   8 12   [0,0] [1,0] [2,0] [3,0]
		// xy yy zy wy = 1  5   9 13 = [0,1] [1,1] [2,1] [3,1]
		// xz yz zz wz   2  6  10 14   [0,2] [1,2] [2,2] [3,2]
		// xw yw zw ww   3  7  11 15   [0,3] [1,3] [2,3] [3,3]
		Float4x4();
		Float4x4(float value);
		Float4x4(const float* array); // Initialize from a 1D array
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

		// Object math operations:
		Float4x4 Transpose();
		float Determinant() const;
		Float4x4 Inverse() const;
		bool IsEpsilonZero() const;

		// Static math operations:
		static Float4x4 RotationX(float angle);
		static Float4x4 RotationY(float angle);
		static Float4x4 RotationZ(float angle);
		static Float4x4 Rotation(const Float3& axis, float angle);
		static Float4x4 RotationFromTo(const Float3& from, const Float3& to);
		static Float4x4 Translate(const Float3& translation);
		static Float4x4 Scale(const Float3& scale);
		static Float4x4 Scale(float scale);

		// Access:
		float& operator[](const Index2& index);
		float operator[](const Index2& index) const;
		Float4 Row(int index) const;
		Float4 Column(int index) const;

		// Assignment:
		Float4x4& operator=(const Float4x4& other);
		Float4x4& operator=(Float4x4&& other) noexcept;

		// Addition:
		Float4x4 operator+(const Float4x4& other) const;
		Float4x4& operator+=(const Float4x4& other);

		// Substraction:
		Float4x4 operator-(const Float4x4& other) const;
		Float4x4& operator-=(const Float4x4& other);

		// Multiplication:
		Float4x4 operator*(const Float4x4& other) const;
		Float4x4& operator*=(const Float4x4& other);
		Float4x4 operator*(float scalar) const;
		Float4x4& operator*=(float scalar);

		// Division:
		Float4x4 operator/(float scalar) const;
		Float4x4& operator/=(float scalar);

		// Comparison:
		bool IsEpsilonEqual(const Float4x4& other) const;
		bool operator==(const Float4x4& other) const;
		bool operator!=(const Float4x4& other) const;

		// Friend functions:
		friend Float4x4 operator*(float a, const Float4x4& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Float4x4& value);

		// Static members:
		static Float4x4 zero;
		static Float4x4 identity;
	};
};



#endif // __INCLUDE_GUARD_mathf_h__