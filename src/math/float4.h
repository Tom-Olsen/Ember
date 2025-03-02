#ifndef __INCLUDE_GUARD_float4_h__
#define __INCLUDE_GUARD_float4_h__
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct Float2;
	struct Float3;



	struct Float4
	{
	public:
		// Members:
		float x, y, z, w;

		// Constructors:
		Float4();
		Float4(float xyzw);
		Float4(float x, float y);
		Float4(float x, float y, float z);
		Float4(float x, float y, float z, float w);
		explicit Float4(const Float2& xy);
		explicit Float4(const Float2& xy, float z);
		explicit Float4(const Float2& xy, float z, float w);
		explicit Float4(const Float2& xy, Float2 zw);
		explicit Float4(const Float3& xyz);
		explicit Float4(const Float3& xyz, float w);
		Float4(const Float4& xyzw);
		static Float4 Direction(float theta, float phi);

		// Math operations:
		float LengthSq() const;
		float Length() const;
		bool IsEpsilonZero() const;

		// Static math operations:
		static Float4 Abs(const Float4& a);
		static Float4 Round(const Float4& value, int decimals = 0);
		static Float4 Min(const Float4& a, const Float4& b);
		static Float4 Max(const Float4& a, const Float4& b);
		static Float4 Clamp(const Float4& value, const Float4& min, const Float4& max);

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
		Float4 operator-() const;

		// Multiplication:
		Float4 operator*(const Float4& other) const;
		Float4& operator*=(const Float4& other);
		Float4& operator*=(float scalar);

		// Division:
		Float4 operator/(const Float4& other) const;
		Float4& operator/=(const Float4& other);
		Float4& operator/=(float scalar);

		// Comparison:
		bool IsEpsilonEqual(const Float4& other) const;
		bool operator==(const Float4& other) const;
		bool operator!=(const Float4& other) const;

		// Friend functions:
		friend Float4 operator*(const Float4& a, float b);
		friend Float4 operator*(float a, const Float4& b);
		friend Float4 operator/(const Float4& a, float b);
		friend Float4 operator/(float a, const Float4& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Float4& value);

		// Static members:
		// Numbers:
		static Float4 zero;		// (0, 0, 0, 0).
		static Float4 one;		// (1, 1, 1, 1).
		static Float4 maxValue;	// math::maxValue * (1, 1, 1, 1).
		static Float4 minValue;	// math::minValue * (1, 1, 1, 1).

		// Directions:
		static Float4 right;	// +x = ( 1, 0, 0, 0).
		static Float4 left;		// -x = (-1, 0, 0, 0).
		static Float4 forward;	// +y = ( 0, 1, 0, 0).
		static Float4 back;		// -y = ( 0,-1, 0, 0).
		static Float4 up;		// +z = ( 0, 0, 1, 0).
		static Float4 down;		// -z = ( 0, 0,-1, 0).
		static Float4 in;		// +w = ( 0, 0, 0, 1).
		static Float4 out;		// -w = ( 0, 0, 0,-1).

		// Colors:
		static Float4 white;	// ( 1, 1, 1, 1).
		static Float4 gray;		// ( 0.5, 0.5, 0.5, 1).
		static Float4 black;	// ( 0, 0, 0, 1).
		static Float4 red;		// ( 1, 0, 0, 1).
		static Float4 green;	// ( 0, 1, 0, 1).
		static Float4 blue;		// ( 0, 0, 1, 1).
		static Float4 yellow;	// ( 1, 1, 0, 1).
		static Float4 cyan;		// ( 0, 1, 1, 1).
		static Float4 magenta;	// ( 1, 0, 1, 1).
	};

	// Friend functions:
	Float4 operator/(float scalar, const Float4& vector);
}



#endif // __INCLUDE_GUARD_float4_h__