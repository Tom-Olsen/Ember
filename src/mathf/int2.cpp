#include "int2.h"
#include <stdexcept>
#include <sstream>



// Constructors:
Int2::Int2() : x(0), y(0) {}
Int2::Int2(int xy) : x(xy), y(xy) {}
Int2::Int2(int x, int y) : x(x), y(y) {}
Int2::Int2(const Int2& xy) : x(xy.x), y(xy.y) {}



// Math operations:



// Static math operations:



// Access:
int& Int2::operator[](int index)
{
	if (index == 0) return x;
	if (index == 1) return y;
	throw std::out_of_range("Int2 index out of range.");
}
int Int2::operator[](int index) const
{
	if (index == 0) return x;
	if (index == 1) return y;
	throw std::out_of_range("Int2 index out of range.");
}



// Assignment:
Int2& Int2::operator=(const Int2& other)
{
	if (this != &other)
	{
		x = other.x;
		y = other.y;
	}
	return *this;
}
Int2& Int2::operator=(Int2&& other) noexcept
{
	if (this != &other)
	{
		x = other.x;
		y = other.y;
	}
	return *this;
}



// Addition:
Int2 Int2::operator+(const Int2& other) const
{
	return Int2(x + other.x, y + other.y);
}
Int2& Int2::operator+=(const Int2& other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}



// Substraction:
Int2 Int2::operator-(const Int2& other) const
{
	return Int2(x - other.x, y - other.y);
}
Int2& Int2::operator-=(const Int2& other)
{
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}
Int2 Int2::operator-() const
{
	return Int2(-x, -y);
}



// Multiplication:
Int2 Int2::operator*(const Int2& other) const
{
	return Int2(x * other.x, y * other.y);
}
Int2& Int2::operator*=(const Int2& other)
{
	this->x *= other.x;
	this->y *= other.y;
	return *this;
}
Int2 Int2::operator*(int scalar) const
{
	return Int2(x * scalar, y * scalar);
}
Int2& Int2::operator*=(int scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}



// Comparison:
bool Int2::operator==(const Int2& other) const
{
	return x == other.x && y == other.y;
}
bool Int2::operator!=(const Int2& other) const
{
	return !(*this == other);
}



// Friend functions:
Int2 operator*(int a, const Int2& b)
{
	return Int2(a * b.x, a * b.y);
}



// Logging:
std::string Int2::ToString() const
{
	std::ostringstream oss;
	oss << "(" << x << ", " << y << ")";
	return oss.str();
}
std::ostream& operator<<(std::ostream& os, const Int2& value)
{
	os << value.ToString();
	return os;
}



// Static members:
Int2 Int2::zero = Int2(0);
Int2 Int2::one = Int2(1);
Int2 Int2::right = Int2(1, 0);
Int2 Int2::left = Int2(-1, 0);
Int2 Int2::up = Int2(0, 1);
Int2 Int2::down = Int2(0, -1);