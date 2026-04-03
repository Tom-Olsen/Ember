#include "int2.h"
#include "float2.h"
#include "mathFunctions.h"
#include <cassert>
#include <sstream>



namespace emberMath
{
	// Constructors:
	Int2::Int2() : x(0), y(0) {}
	Int2::Int2(int xy) : x(xy), y(xy) {}
	Int2::Int2(int x, int y) : x(x), y(y) {}
	Int2::Int2(const Int2& xy) : x(xy.x), y(xy.y) {}
	Int2::Int2(const Float2& xy) : x(xy.x), y(xy.y) {}



	// Math operations:



	// Static math operations:
	Int2 Int2::Min(const Int2& a, const Int2& b)
	{
		return Int2(math::Min(a.x, b.x), math::Min(a.y, b.y));
	}
	Int2 Int2::Max(const Int2& a, const Int2& b)
	{
		return Int2(math::Max(a.x, b.x), math::Max(a.y, b.y));
	}
	Int2 Int2::Clamp(const Int2& value, const Int2& min, const Int2& max)
	{
        assert(min < max);
		return Int2(math::Clamp(value.x, min.x, max.x), math::Clamp(value.y, min.y, max.y));
	}



	// Access:
	int& Int2::operator[](int index)
	{
        assert(index >= 0 && index < 2);
		if (index == 0) return x;
		if (index == 1) return y;
	}
	int Int2::operator[](int index) const
	{
        assert(index >= 0 && index < 2);
		if (index == 0) return x;
		if (index == 1) return y;
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
    bool Int2::operator<(const Int2& other) const
    {
        return x < other.x && y < other.y;
    }
    bool Int2::operator<=(const Int2& other) const
    {
        return x <= other.x && y <= other.y;
    }
    bool Int2::operator>(const Int2& other) const
    {
        return x > other.x && y > other.y;
    }
    bool Int2::operator>=(const Int2& other) const
    {
        return x >= other.x && y >= other.y;
    }



	// Friend functions:
	Int2 operator*(int a, const Int2& b)
	{
		return Int2(a * b.x, a * b.y);
	}
	Int2 operator*(const Int2& a, int b)
	{
		return Int2(a.x * b, a.y * b);
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
}