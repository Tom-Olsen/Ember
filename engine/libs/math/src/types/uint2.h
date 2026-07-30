#pragma once
#include <cstdint>
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct Int2;
	struct Float2;



	// Mainly used for vulkan triangle indices.
	struct Uint2
	{
	public:
		// Members:
        static constexpr int size = 2;
		uint32_t x, y;

		// Constructors:
		constexpr Uint2() : x(0), y(0) {}
		constexpr Uint2(uint32_t xy) : x(xy), y(xy) {}
		constexpr Uint2(uint32_t x, uint32_t y) : x(x), y(y) {}
		Uint2(const Uint2& xy);
		Uint2(const Int2& xy);
		Uint2(const Float2& xy);

		// Math operations:

		// Static math operations:
		static Uint2 Min(const Uint2& a, const Uint2& b);
		static Uint2 Max(const Uint2& a, const Uint2& b);
		static Uint2 Clamp(const Uint2& value, const Uint2& min, const Uint2& max);

		// Access:
		uint32_t& operator[](int index);
		uint32_t operator[](int index) const;

		// Assignment:
		Uint2& operator=(const Uint2& other);
		Uint2& operator=(Uint2&& other) noexcept;

		// Addition:
		Uint2 operator+(const Uint2& other) const;
		Uint2& operator+=(const Uint2& other);

		// Substraction:
		Uint2 operator-(const Uint2& other) const;
		Uint2& operator-=(const Uint2& other);

		// Multiplication:
		Uint2 operator*(const Uint2& other) const;
		Uint2& operator*=(const Uint2& other);
		Uint2 operator*(int scalar) const;
		Uint2& operator*=(int scalar);
		
		// Comparison:
		bool operator==(const Uint2& other) const;
		bool operator!=(const Uint2& other) const;
        bool operator<(const Uint2& other) const;
        bool operator<=(const Uint2& other) const;
        bool operator>(const Uint2& other) const;
        bool operator>=(const Uint2& other) const;

		// Friend functions:
		friend Uint2 operator*(int a, const Uint2& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Uint2& value);

		// Static members:
		static const Uint2 zero;
		static const Uint2 one;
		static const Uint2 right;		// +x = ( 1, 0, 0).
		static const Uint2 forward;		// +y = ( 0, 1, 0).
	};

	// Static members:
	inline constexpr Uint2 Uint2::zero(0);
	inline constexpr Uint2 Uint2::one(1);
	inline constexpr Uint2 Uint2::right(1, 0);
	inline constexpr Uint2 Uint2::forward(0, 1);
}