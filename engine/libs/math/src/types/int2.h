#pragma once
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct Float2;
    struct Uint2;



	struct Int2
	{
	public:
		// Members:
        static constexpr int size = 2;
		int x, y;

		// Constructors:
		constexpr Int2() : x(0), y(0) {}
		constexpr Int2(int xy) : x(xy), y(xy) {}
		constexpr Int2(int x, int y) : x(x), y(y) {}
		Int2(const Int2& xy);
        Int2(const Uint2& xy);
		Int2(const Float2& xy);

		// Math operations:

		// Static math operations:
		static Int2 Min(const Int2& a, const Int2& b);
		static Int2 Max(const Int2& a, const Int2& b);
		static Int2 Clamp(const Int2& value, const Int2& min, const Int2& max);

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
		Int2 operator-() const;

		// Multiplication:
		Int2 operator*(const Int2& other) const;
		Int2& operator*=(const Int2& other);
		Int2& operator*=(int scalar);

		// Comparison:
		bool operator==(const Int2& other) const;
		bool operator!=(const Int2& other) const;
        bool operator<(const Int2& other) const;
        bool operator<=(const Int2& other) const;
        bool operator>(const Int2& other) const;
        bool operator>=(const Int2& other) const;

		// Friend functions:
		friend Int2 operator*(int a, const Int2& b);
		friend Int2 operator*(const Int2& a, int b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Int2& value);

		// Static members:
		static const Int2 zero;
		static const Int2 one;
		static const Int2 right;	// +x = ( 1, 0).
		static const Int2 left;		// -x = (-1, 0).
		static const Int2 up;		// +y = ( 0, 1).
		static const Int2 down;		// -y = ( 0,-1).
	};

	// Static members:
	inline constexpr Int2 Int2::zero(0);
	inline constexpr Int2 Int2::one(1);
	inline constexpr Int2 Int2::right(1, 0);
	inline constexpr Int2 Int2::left(-1, 0);
	inline constexpr Int2 Int2::up(0, 1);
	inline constexpr Int2 Int2::down(0, -1);
}