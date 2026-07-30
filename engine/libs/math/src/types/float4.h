#pragma once
#include "mathConstants.h"
#include <cstdint>
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
        static constexpr int size = 4;
		float x, y, z, w;

		// Constructors:
		constexpr Float4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		constexpr Float4(float xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}
		constexpr Float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		explicit Float4(const Float2& xy, float z, float w);
		explicit Float4(const Float2& xy, Float2 zw);
		explicit Float4(const Float3& xyz, float w);
		Float4(const Float4& xyzw);
		static Float4 Direction(float theta, float phi);

		// Math operations:
		float LengthSq() const;
		float Length() const;
		bool IsEpsilonZero(float absEpsilon = math::absEpsilon) const;

		// Static math operations:
		static Float4 Abs(const Float4& a);
		static Float4 Round(const Float4& value, uint8_t decimals = 0);
		static Float4 Ceil(const Float4& value, uint8_t decimals = 0);
		static Float4 Floor(const Float4& value, uint8_t decimals = 0);
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
		bool IsEpsilonEqual(const Float4& other, float absEpsilon = math::absEpsilon, float relEpsilon = math::relEpsilon) const;
		bool operator==(const Float4& other) const;
		bool operator!=(const Float4& other) const;
        bool operator<(const Float4& other) const;
        bool operator<=(const Float4& other) const;
        bool operator>(const Float4& other) const;
        bool operator>=(const Float4& other) const;

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
		static const Float4 zero;		// (0, 0, 0, 0).
		static const Float4 one;		// (1, 1, 1, 1).
		static const Float4 maxValue;	// math::maxValue * (1, 1, 1, 1).
		static const Float4 minValue;	// math::minValue * (1, 1, 1, 1).

		// Directions:
		static const Float4 right;		// +x = ( 1, 0, 0, 0).
		static const Float4 left;		// -x = (-1, 0, 0, 0).
		static const Float4 forward;	// +y = ( 0, 1, 0, 0).
		static const Float4 back;		// -y = ( 0,-1, 0, 0).
		static const Float4 up;			// +z = ( 0, 0, 1, 0).
		static const Float4 down;		// -z = ( 0, 0,-1, 0).
		static const Float4 in;			// +w = ( 0, 0, 0, 1).
		static const Float4 out;		// -w = ( 0, 0, 0,-1).

		// Colors:
		static const Float4 white;		// ( 1, 1, 1, 1).
		static const Float4 gray;		// ( 0.5, 0.5, 0.5, 1).
		static const Float4 black;		// ( 0, 0, 0, 1).
		static const Float4 red;		// ( 1, 0, 0, 1).
		static const Float4 green;		// ( 0, 1, 0, 1).
		static const Float4 blue;		// ( 0, 0, 1, 1).
		static const Float4 yellow;		// ( 1, 1, 0, 1).
		static const Float4 cyan;		// ( 0, 1, 1, 1).
		static const Float4 magenta;	// ( 1, 0, 1, 1).
		static const Float4 orange;		// ( 1, 0.5, 0, 1).
	};

	// Static members:
	// Numbers:
	inline constexpr Float4 Float4::zero(0.0f);
	inline constexpr Float4 Float4::one(1.0f);
	inline constexpr Float4 Float4::maxValue(math::maxValue);
	inline constexpr Float4 Float4::minValue(math::minValue);

	// Directions:
	inline constexpr Float4 Float4::right(1.0f, 0.0f, 0.0f, 0.0f);
	inline constexpr Float4 Float4::left(-1.0f, 0.0f, 0.0f, 0.0f);
	inline constexpr Float4 Float4::forward(0.0f, 1.0f, 0.0f, 0.0f);
	inline constexpr Float4 Float4::back(0.0f, -1.0f, 0.0f, 0.0f);
	inline constexpr Float4 Float4::up(0.0f, 0.0f, 1.0f, 0.0f);
	inline constexpr Float4 Float4::down(0.0f, 0.0f, -1.0f, 0.0f);
	inline constexpr Float4 Float4::in(0.0f, 0.0f, 0.0f, 1.0f);
	inline constexpr Float4 Float4::out(0.0f, 0.0f, 0.0f, -1.0f);

	// Colors:
	inline constexpr Float4 Float4::white(1.0f, 1.0f, 1.0f, 1.0f);
	inline constexpr Float4 Float4::gray(0.5f, 0.5f, 0.5f, 1.0f);
	inline constexpr Float4 Float4::black(0.0f, 0.0f, 0.0f, 1.0f);
	inline constexpr Float4 Float4::red(1.0f, 0.0f, 0.0f, 1.0f);
	inline constexpr Float4 Float4::green(0.0f, 1.0f, 0.0f, 1.0f);
	inline constexpr Float4 Float4::blue(0.0f, 0.0f, 1.0f, 1.0f);
	inline constexpr Float4 Float4::yellow(1.0f, 1.0f, 0.0f, 1.0f);
	inline constexpr Float4 Float4::cyan(0.0f, 1.0f, 1.0f, 1.0f);
	inline constexpr Float4 Float4::magenta(1.0f, 0.0f, 1.0f, 1.0f);
	inline constexpr Float4 Float4::orange(1.0f, 0.5f, 0.0f, 1.0f);

	// Friend functions:
	Float4 operator/(float scalar, const Float4& vector);
}