#include "uint3.h"
#include "mathFunctions.h"
#include <stdexcept>
#include <sstream>



namespace emberMath
{
	// Constructors:
	Uint3::Uint3() : x(0), y(0), z(0) {}
	Uint3::Uint3(uint32_t xyz) : x(xyz), y(xyz), z(xyz) {}
	Uint3::Uint3(uint32_t x, uint32_t y, uint32_t z) : x(x), y(y), z(z) {}
	Uint3::Uint3(const Uint3& xyz) : x(xyz.x), y(xyz.y), z(xyz.z) {}



	// Math operations:



	// Static math operations:
	Uint3 Uint3::Min(const Uint3& a, const Uint3& b)
	{
		return Uint3(math::Min(a.x, b.x), math::Min(a.y, b.y), math::Min(a.z, b.z));
	}
	Uint3 Uint3::Max(const Uint3& a, const Uint3& b)
	{
		return Uint3(math::Max(a.x, b.x), math::Max(a.y, b.y), math::Max(a.z, b.z));
	}



	// Access:
	uint32_t& Uint3::operator[](int index)
	{
		if (index == 0) return x;
		if (index == 1) return y;
		if (index == 2) return z;
		throw std::out_of_range("Uint3 index out of range.");
	}
	uint32_t Uint3::operator[](int index) const
	{
		if (index == 0) return x;
		if (index == 1) return y;
		if (index == 2) return z;
		throw std::out_of_range("Uint3 index out of range.");
	}



	// Assignment:
	Uint3& Uint3::operator=(const Uint3& other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}
		return *this;
	}
	Uint3& Uint3::operator=(Uint3&& other) noexcept
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}
		return *this;
	}



	// Addition:
	Uint3 Uint3::operator+(const Uint3& other) const
	{
		return Uint3(x + other.x, y + other.y, z + other.z);
	}
	Uint3& Uint3::operator+=(const Uint3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}



	// Substraction:
	Uint3 Uint3::operator-(const Uint3& other) const
	{
		return Uint3(x - other.x, y - other.y, z - other.z);
	}
	Uint3& Uint3::operator-=(const Uint3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}



	// Multiplication:
	Uint3 Uint3::operator*(const Uint3& other) const
	{
		return Uint3(x * other.x, y * other.y, z * other.z);
	}
	Uint3& Uint3::operator*=(const Uint3& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;
		return *this;
	}
	Uint3 Uint3::operator*(int scalar) const
	{
		return Uint3(x * scalar, y * scalar, z * scalar);
	}
	Uint3& Uint3::operator*=(int scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}



	// Comparison:
	bool Uint3::operator==(const Uint3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}
	bool Uint3::operator!=(const Uint3& other) const
	{
		return !(*this == other);
	}



	// Friend functions:
	Uint3 operator*(int a, const Uint3& b)
	{
		return Uint3(a * b.x, a * b.y, a * b.z);
	}



	// Logging:
	std::string Uint3::ToString() const
	{
		std::ostringstream oss;
		oss << "(" << x << ", " << y << ", " << z << ")";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Uint3& value)
	{
		os << value.ToString();
		return os;
	}



	// Static members:
	Uint3 Uint3::zero = Uint3(0);			// zero = Uint3(0,0,0).
	Uint3 Uint3::one = Uint3(1);			// one = Uint3(1,1,1).
	Uint3 Uint3::right = Uint3(1, 0, 0);	// right = Uint3(1,0,0).
	Uint3 Uint3::up = Uint3(0, 1, 0);		// up = Uint3(0,1,0).
	Uint3 Uint3::forward = Uint3(0, 0, 1);	// forward = Uint3(0,0,1).
}