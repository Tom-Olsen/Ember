#pragma once
#include <cstdint>
#include <string>


namespace emberMath
{
	// Forward declarations:
	struct Float2;
	struct Float3;
	struct Int2;
	struct Int3;
	struct Uint2;



	// Mainly used for vulkan triangle indices.
	struct Uint3
	{
	public:
		// Members:
        static constexpr int size = 3;
		uint32_t x, y, z;

		// Constructors:
		constexpr Uint3() : x(0), y(0), z(0) {}
		constexpr Uint3(uint32_t xyz) : x(xyz), y(xyz), z(xyz) {}
		constexpr Uint3(uint32_t x, uint32_t y, uint32_t z) : x(x), y(y), z(z) {}
		Uint3(const Uint2& xy, uint32_t z);
		Uint3(const Int2& xy, uint32_t z);
		Uint3(const Float2& xy, uint32_t z);
		Uint3(const Uint3& xyz);
		Uint3(const Int3& xyz);
		Uint3(const Float3& xyz);

		// Math operations:

		// Static math operations:
		static Uint3 Min(const Uint3& a, const Uint3& b);
		static Uint3 Max(const Uint3& a, const Uint3& b);
		static Uint3 Clamp(const Uint3& value, const Uint3& min, const Uint3& max);

		// Access:
		uint32_t& operator[](int index);
		uint32_t operator[](int index) const;

		// Assignment:
		Uint3& operator=(const Uint3& other);
		Uint3& operator=(Uint3&& other) noexcept;

		// Addition:
		Uint3 operator+(const Uint3& other) const;
		Uint3& operator+=(const Uint3& other);

		// Substraction:
		Uint3 operator-(const Uint3& other) const;
		Uint3& operator-=(const Uint3& other);

		// Multiplication:
		Uint3 operator*(const Uint3& other) const;
		Uint3& operator*=(const Uint3& other);
		Uint3 operator*(int scalar) const;
		Uint3& operator*=(int scalar);

		// Comparison:
		bool operator==(const Uint3& other) const;
		bool operator!=(const Uint3& other) const;
        bool operator<(const Uint3& other) const;
        bool operator<=(const Uint3& other) const;
        bool operator>(const Uint3& other) const;
        bool operator>=(const Uint3& other) const;

		// Friend functions:
		friend Uint3 operator*(int a, const Uint3& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Uint3& value);

		// Static members:
		static const Uint3 zero;
		static const Uint3 one;
		static const Uint3 right;		// +x = ( 1, 0, 0).
		static const Uint3 forward;		// +y = ( 0, 1, 0).
		static const Uint3 up;			// +z = ( 0, 0, 1).
	};

	// Static members:
	inline constexpr Uint3 Uint3::zero(0);
	inline constexpr Uint3 Uint3::one(1);
	inline constexpr Uint3 Uint3::right(1, 0, 0);
	inline constexpr Uint3 Uint3::forward(0, 1, 0);
	inline constexpr Uint3 Uint3::up(0, 0, 1);
}