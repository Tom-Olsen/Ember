#ifndef __INCLUDE_GUARD_uint3_h__
#define __INCLUDE_GUARD_uint3_h__
#include <string>



namespace emberMath
{
	// Mainly used for vulkan triangle indices.
	struct Uint3
	{
	public:
		// Members:
		uint32_t x, y, z;

		// Constructors:
		Uint3();
		Uint3(uint32_t xyz);
		Uint3(uint32_t x, uint32_t y, uint32_t z);
		Uint3(const Uint3& xyz);

		// Math operations:

		// Static math operations:
		static Uint3 Min(const Uint3& a, const Uint3& b);
		static Uint3 Max(const Uint3& a, const Uint3& b);

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

		// Friend functions:
		friend Uint3 operator*(int a, const Uint3& b);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Uint3& value);

		// Static members:
		static Uint3 zero;
		static Uint3 one;
		static Uint3 right;		// +x = ( 1, 0, 0).
		static Uint3 up;		// +y = ( 0, 1, 0).
		static Uint3 forward;	// +z = ( 0, 0, 1).
	};
}



#endif // __INCLUDE_GUARD_uint3_h__