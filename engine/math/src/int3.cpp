#include "int3.h"
#include "int2.h"
#include <stdexcept>
#include <sstream>



namespace emberMath
{
	// Constructors:
	Int3::Int3() : x(0), y(0), z(0) {}
	Int3::Int3(int xyz) : x(xyz), y(xyz), z(xyz) {}
	Int3::Int3(int x, int y) : x(x), y(y), z(0) {}
	Int3::Int3(int x, int y, int z) : x(x), y(y), z(z) {}
	Int3::Int3(const Int2& xy) : x(xy.x), y(xy.y), z(0) {}
	Int3::Int3(const Int2& xy, int z) : x(xy.x), y(xy.y), z(z) {}
	Int3::Int3(const Int3& xyz) : x(xyz.x), y(xyz.y), z(xyz.z) {}



	// Math operations:



	// Static math operations:



	// Access:
	int& Int3::operator[](int index)
	{
		if (index == 0) return x;
		if (index == 1) return y;
		if (index == 2) return z;
		throw std::out_of_range("Int3 index out of range.");
	}
	int Int3::operator[](int index) const
	{
		if (index == 0) return x;
		if (index == 1) return y;
		if (index == 2) return z;
		throw std::out_of_range("Int3 index out of range.");
	}



	// Assignment:
	Int3& Int3::operator=(const Int3& other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}
		return *this;
	}
	Int3& Int3::operator=(Int3&& other) noexcept
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
	Int3 Int3::operator+(const Int3& other) const
	{
		return Int3(x + other.x, y + other.y, z + other.z);
	}
	Int3& Int3::operator+=(const Int3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}



	// Substraction:
	Int3 Int3::operator-(const Int3& other) const
	{
		return Int3(x - other.x, y - other.y, z - other.z);
	}
	Int3& Int3::operator-=(const Int3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}
	Int3 Int3::operator-() const
	{
		return Int3(-x, -y, -z);
	}



	// Multiplication:
	Int3 Int3::operator*(const Int3& other) const
	{
		return Int3(x * other.x, y * other.y, z * other.z);
	}
	Int3& Int3::operator*=(const Int3& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;
		return *this;
	}
	Int3 Int3::operator*(int scalar) const
	{
		return Int3(x * scalar, y * scalar, z * scalar);
	}
	Int3& Int3::operator*=(int scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}



	// Comparison:
	bool Int3::operator==(const Int3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}
	bool Int3::operator!=(const Int3& other) const
	{
		return !(*this == other);
	}



	// Friend functions:
	Int3 operator*(int a, const Int3& b)
	{
		return Int3(a * b.x, a * b.y, a * b.z);
	}



	// Logging:
	std::string Int3::ToString() const
	{
		std::ostringstream oss;
		oss << "(" << x << ", " << y << ", " << z << ")";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Int3& value)
	{
		os << value.ToString();
		return os;
	}



	// Static members:
	Int3 Int3::zero = Int3(0);
	Int3 Int3::one = Int3(1);
	Int3 Int3::right = Int3(1, 0, 0);
	Int3 Int3::left = Int3(-1, 0, 0);
	Int3 Int3::up = Int3(0, 1, 0);
	Int3 Int3::down = Int3(0, -1, 0);
	Int3 Int3::forward = Int3(0, 0, 1);
	Int3 Int3::backward = Int3(0, 0, -1);
}