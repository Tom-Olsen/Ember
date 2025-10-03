#include "uint2.h"
#include "mathFunctions.h"
#include <stdexcept>
#include <sstream>



namespace emberMath
{
	// Constructors:
	Uint2::Uint2() : x(0), y(0) {}
	Uint2::Uint2(uint32_t xy) : x(xy), y(xy) {}
	Uint2::Uint2(uint32_t x, uint32_t y) : x(x), y(y) {}
	Uint2::Uint2(const Uint2& xy) : x(xy.x), y(xy.y) {}



	// Math operations:



	// Static math operations:
	Uint2 Uint2::Min(const Uint2& a, const Uint2& b)
	{
		return Uint2(math::Min(a.x, b.x), math::Min(a.y, b.y));
	}
	Uint2 Uint2::Max(const Uint2& a, const Uint2& b)
	{
		return Uint2(math::Max(a.x, b.x), math::Max(a.y, b.y));
	}



	// Access:
	uint32_t& Uint2::operator[](int index)
	{
		if (index == 0) return x;
		if (index == 1) return y;
		throw std::out_of_range("Uint2 index out of range.");
	}
	uint32_t Uint2::operator[](int index) const
	{
		if (index == 0) return x;
		if (index == 1) return y;
		throw std::out_of_range("Uint2 index out of range.");
	}



	// Assignment:
	Uint2& Uint2::operator=(const Uint2& other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
		}
		return *this;
	}
	Uint2& Uint2::operator=(Uint2&& other) noexcept
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
		}
		return *this;
	}



	// Addition:
	Uint2 Uint2::operator+(const Uint2& other) const
	{
		return Uint2(x + other.x, y + other.y);
	}
	Uint2& Uint2::operator+=(const Uint2& other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}



	// Substraction:
	Uint2 Uint2::operator-(const Uint2& other) const
	{
		return Uint2(x - other.x, y - other.y);
	}
	Uint2& Uint2::operator-=(const Uint2& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}



	// Multiplication:
	Uint2 Uint2::operator*(const Uint2& other) const
	{
		return Uint2(x * other.x, y * other.y);
	}
	Uint2& Uint2::operator*=(const Uint2& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		return *this;
	}
	Uint2 Uint2::operator*(int scalar) const
	{
		return Uint2(x * scalar, y * scalar);
	}
	Uint2& Uint2::operator*=(int scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}



	// Comparison:
	bool Uint2::operator==(const Uint2& other) const
	{
		return x == other.x && y == other.y;
	}
	bool Uint2::operator!=(const Uint2& other) const
	{
		return !(*this == other);
	}



	// Friend functions:
	Uint2 operator*(int a, const Uint2& b)
	{
		return Uint2(a * b.x, a * b.y);
	}



	// Logging:
	std::string Uint2::ToString() const
	{
		std::ostringstream oss;
		oss << "(" << x << ", " << y << ")";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Uint2& value)
	{
		os << value.ToString();
		return os;
	}



	// Static members:
	Uint2 Uint2::zero = Uint2(0);			// zero = Uint2(0,0).
	Uint2 Uint2::one = Uint2(1);			// one = Uint2(1,1).
	Uint2 Uint2::right = Uint2(1, 0);		// right = Uint2(1,0).
	Uint2 Uint2::forward = Uint2(0, 1);		// forward = Uint2(0,1).
}