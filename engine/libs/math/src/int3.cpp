#include "int3.h"
#include "int2.h"
#include "mathFunctions.h"
#include <cassert>
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
	Int3 Int3::Min(const Int3& a, const Int3& b)
	{
		return Int3(math::Min(a.x, b.x), math::Min(a.y, b.y), math::Min(a.z, b.z));
	}
	Int3 Int3::Max(const Int3& a, const Int3& b)
	{
		return Int3(math::Max(a.x, b.x), math::Max(a.y, b.y), math::Max(a.z, b.z));
	}
	Int3 Int3::Clamp(const Int3& value, const Int3& min, const Int3& max)
	{
        assert(min < max);
		return Int3(math::Clamp(value.x, min.x, max.x), math::Clamp(value.y, min.y, max.y), math::Clamp(value.z, min.z, max.z));
	}



	// Access:
	int& Int3::operator[](int index)
	{
        assert(index >= 0 && index < 3);
		return (&x)[index];
	}
	int Int3::operator[](int index) const
	{
        assert(index >= 0 && index < 3);
		return (&x)[index];
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
    bool Int3::operator<(const Int3& other) const
    {
        return x < other.x && y < other.y && z < other.z;
    }
    bool Int3::operator<=(const Int3& other) const
    {
        return x <= other.x && y <= other.y && z <= other.z;
    }
    bool Int3::operator>(const Int3& other) const
    {
        return x > other.x && y > other.y && z > other.z;
    }
    bool Int3::operator>=(const Int3& other) const
    {
        return x >= other.x && y >= other.y && z >= other.z;
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