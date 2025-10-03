#pragma once
#include <string>



namespace emberMath
{
	// Mainly used for vulkan triangle indices.
	struct Uint2
	{
	public:
		// Members:
		uint32_t x, y;

		// Constructors:
		Uint2();
		Uint2(uint32_t xy);
		Uint2(uint32_t x, uint32_t y);
		Uint2(const Uint2& xy);

		// Math operations:

		// Static math operations:
		static Uint2 Min(const Uint2& a, const Uint2& b);
		static Uint2 Max(const Uint2& a, const Uint2& b);

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

		// Friend functions:
		friend Uint2 operator*(int a, const Uint2& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Uint2& value);

		// Static members:
		static Uint2 zero;
		static Uint2 one;
		static Uint2 right;		// +x = ( 1, 0, 0).
		static Uint2 forward;	// +y = ( 0, 1, 0).
	};
}