#ifndef __INCLUDE_GUARD_float3_h__
#define __INCLUDE_GUARD_float3_h__
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct Float2;
	struct Float4;



	struct Float3
	{
	public:
		// Members:
		float x, y, z;

		// Constructors:
		Float3();
		Float3(float xyz);
		Float3(float x, float y);
		Float3(float x, float y, float z);
		explicit Float3(const Float2& xy);
		explicit Float3(const Float2& xy, float z);
		Float3(const Float3& xyz);
		explicit Float3(const Float4& xyz);
		static Float3 Direction(float theta, float phi);

		// Math operations:
		float LengthSq() const;
		float Length() const;
		float Theta() const;
		float Phi() const;
		Float2 Angles() const;
		Float3 Normalize() const;
		Float3 Rotate(float theta, float phi) const;
		bool IsEpsilonZero() const;

		// Static math operations:
		static Float3 Abs(const Float3& a);
		static Float3 Round(const Float3& value, int decimals = 0);
		static Float3 Ceil(const Float3& value, int decimals = 0);
		static Float3 Floor(const Float3& value, int decimals = 0);
		static float Dot(const Float3& a, const Float3& b);
		static Float3 Cross(const Float3& a, const Float3& b);
		static float DistanceSq(const Float3& a, const Float3& b);
		static float Distance(const Float3& a, const Float3& b);
		static float Angle(const Float3& a, const Float3& b);
		static Float3 Min(const Float3& a, const Float3& b);
		static Float3 Max(const Float3& a, const Float3& b);
		static Float3 Clamp(const Float3& value, const Float3& min, const Float3& max);

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
		Float3 operator-() const;

		// Multiplication:
		Float3 operator*(const Float3& other) const;
		Float3& operator*=(const Float3& other);
		Float3& operator*=(float scalar);

		// Division:
		Float3 operator/(const Float3& other) const;
		Float3& operator/=(const Float3& other);
		Float3& operator/=(float scalar);

		// Comparison:
		bool IsEpsilonEqual(const Float3& other) const;
		bool operator==(const Float3& other) const;
		bool operator!=(const Float3& other) const;

		// Friend functions:
		friend Float3 operator*(const Float3& a, float b);
		friend Float3 operator*(float a, const Float3& b);
		friend Float3 operator/(const Float3& a, float b);
		friend Float3 operator/(float a, const Float3& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Float3& value);

		// Static members:
		// Numbers:
		static Float3 zero;		// (0, 0, 0).
		static Float3 one;		// (1, 1, 1).
		static Float3 maxValue;	// math::maxValue * (1, 1, 1).
		static Float3 minValue;	// math::minValue * (1, 1, 1).

		// Directions:
		static Float3 right;	// +x = ( 1, 0, 0).
		static Float3 left;		// -x = (-1, 0, 0).
		static Float3 forward;	// +y = ( 0, 1, 0).
		static Float3 back;		// -y = ( 0,-1, 0).
		static Float3 up;		// +z = ( 0, 0, 1).
		static Float3 down;		// -z = ( 0, 0,-1).

		// Colors:
		static Float3 white;	// ( 1, 1, 1).
		static Float3 gray;		// ( 0.5, 0.5, 0.5).
		static Float3 black;	// ( 0, 0, 0).
		static Float3 red;		// ( 1, 0, 0).
		static Float3 green;	// ( 0, 1, 0).
		static Float3 blue;		// ( 0, 0, 1).
		static Float3 yellow;	// ( 1, 1, 0).
		static Float3 cyan;		// ( 0, 1, 1).
		static Float3 magenta;	// ( 1, 0, 1).
	};

	// Friend functions:
	Float3 operator/(float scalar, const Float3& vector);
}



#endif // __INCLUDE_GUARD_float3_h__