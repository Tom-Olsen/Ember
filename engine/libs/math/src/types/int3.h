#pragma once
#include "int2.h"
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct Float2;
	struct Float3;
	struct Uint2;
	struct Uint3;



	struct Int3
	{
	public:
		// Members:
        static constexpr int size = 3;
		int x, y, z;

		// Constructors:
		constexpr Int3() : x(0), y(0), z(0) {}
		constexpr Int3(int xyz) : x(xyz), y(xyz), z(xyz) {}
		constexpr Int3(int x, int y, int z) : x(x), y(y), z(z) {}
		Int3(const Int2& xy, int z);
		Int3(const Uint2& xy, int z);
		Int3(const Float2& xy, int z);
		Int3(const Int3& xyz);
		Int3(const Uint3& xyz);
		Int3(const Float3& xyz);

		// Math operations:

		// Static math operations:
		static Int3 Min(const Int3& a, const Int3& b);
		static Int3 Max(const Int3& a, const Int3& b);
		static Int3 Clamp(const Int3& value, const Int3& min, const Int3& max);

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
		Int3 operator-() const;

		// Multiplication:
		Int3 operator*(const Int3& other) const;
		Int3& operator*=(const Int3& other);
		Int3 operator*(int scalar) const;
		Int3& operator*=(int scalar);

		// Comparison:
		bool operator==(const Int3& other) const;
		bool operator!=(const Int3& other) const;
        bool operator<(const Int3& other) const;
        bool operator<=(const Int3& other) const;
        bool operator>(const Int3& other) const;
        bool operator>=(const Int3& other) const;

		// Friend functions:
		friend Int3 operator*(int a, const Int3& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Int3& value);

		// Static members:
		static const Int3 zero;
		static const Int3 one;
		static const Int3 right;		// +x = ( 1, 0, 0).
		static const Int3 left;			// -x = (-1, 0, 0).
		static const Int3 up;			// +y = ( 0, 1, 0).
		static const Int3 down;			// -y = ( 0,-1, 0).
		static const Int3 forward;		// +z = ( 0, 0, 1).
		static const Int3 backward;		// -z = ( 0, 0,-1).
	};

	// Static members:
	inline constexpr Int3 Int3::zero(0);
	inline constexpr Int3 Int3::one(1);
	inline constexpr Int3 Int3::right(1, 0, 0);
	inline constexpr Int3 Int3::left(-1, 0, 0);
	inline constexpr Int3 Int3::up(0, 1, 0);
	inline constexpr Int3 Int3::down(0, -1, 0);
	inline constexpr Int3 Int3::forward(0, 0, 1);
	inline constexpr Int3 Int3::backward(0, 0, -1);
}