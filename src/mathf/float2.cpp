#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "mathf.h"
#include <stdexcept>
#include <sstream>



// Constructors:
Float2::Float2() : x(0), y(0) {}
Float2::Float2(float xy) : x(xy), y(xy) {}
Float2::Float2(float x, float y) : x(x), y(y) {}
Float2::Float2(const Float2& xy) : x(xy.x), y(xy.y) {}
Float2::Float2(const Float3& xy) : x(xy.x), y(xy.y) {}
Float2::Float2(const Float4& xy) : x(xy.x), y(xy.y) {}
Float2 Float2::Direction(float angle)
{
	return Float2(mathf::Cos(angle), mathf::Sin(angle));
}



// Math operations:
float Float2::LengthSq() const
{
	return x * x + y * y;
}
float Float2::Length() const
{
	return sqrt(LengthSq());
}
float Float2::Angle() const
{
	return mathf::Atan2(y, x);
}
Float2 Float2::Normalize() const
{
	float length = Length();
	if (length <= mathf::epsilon)
		return Float2(0.0f);
	return Float2(x / length, y / length);
}
Float2 Float2::Rotate(float angle) const
{
	float c = mathf::Cos(angle);
	float s = mathf::Sin(angle);
	return Float2(x * c - y * s, x * s + y * c);
}
bool Float2::IsEpsilonZero() const
{
	return IsEpsilonEqual(Float2::zero);
}



// Static math operations:
Float2 Float2::Abs(const Float2& a)
{
	return Float2(mathf::Abs(a.x), mathf::Abs(a.y));
}
float Float2::Dot(const Float2& a, const Float2& b)
{
	return a.x * b.x + a.y * b.y;
}
float Float2::Cross(const Float2& a, const Float2& b)
{
	return a.y * b.x - a.x * b.y;
}
float Float2::DistanceSq(const Float2& a, const Float2& b)
{
	return (a - b).LengthSq();
}
float Float2::Distance(const Float2& a, const Float2& b)
{
	return (a - b).Length();
}
float Float2::Angle(const Float2& a, const Float2& b)
{
	float lengthA = a.Length();
	float lengthB = b.Length();
	float lengths = a.Length() * b.Length();
	if (lengthA <= mathf::epsilon || lengthB <= mathf::epsilon)
		return 0.0f;
	return mathf::Acos(mathf::Clamp(Dot(a, b) / lengthA * lengthB, -1.0f, 1.0f));
}
Float2 Float2::Min(const Float2& a, const Float2& b)
{
	return Float2(mathf::Min(a.x, b.x), mathf::Min(a.y, b.y));
}
Float2 Float2::Max(const Float2& a, const Float2& b)
{
	return Float2(mathf::Max(a.x, b.x), mathf::Max(a.y, b.y));
}
Float2 Float2::Clamp(const Float2& value, const Float2& min, const Float2& max)
{
	return Float2(mathf::Clamp(value.x, min.x, max.x), mathf::Clamp(value.y, min.y, max.y));
}



// Access:
float& Float2::operator[](int index)
{
	if (index == 0) return x;
	if (index == 1) return y;
	throw std::out_of_range("Float2 index out of range.");
}
float Float2::operator[](int index) const
{
	if (index == 0) return x;
	if (index == 1) return y;
	throw std::out_of_range("Float2 index out of range.");
}



// Assignment:
Float2& Float2::operator=(const Float2& other)
{
	if (this != &other)
	{
		x = other.x;
		y = other.y;
	}
	return *this;
}
Float2& Float2::operator=(Float2&& other) noexcept
{
	if (this != &other)
	{
		x = other.x;
		y = other.y;
	}
	return *this;
}



// Addition:
Float2 Float2::operator+(const Float2& other) const
{
	return Float2(x + other.x, y + other.y);
}
Float2& Float2::operator+=(const Float2& other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}



// Substraction:
Float2 Float2::operator-(const Float2& other) const
{
	return Float2(x - other.x, y - other.y);
}
Float2& Float2::operator-=(const Float2& other)
{
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}
Float2 Float2::operator-() const
{
	return Float2(-x, -y);
}



// Multiplication:
Float2 Float2::operator*(const Float2& other) const
{
	return Float2(x * other.x, y * other.y);
}
Float2& Float2::operator*=(const Float2& other)
{
	this->x *= other.x;
	this->y *= other.y;
	return *this;
}
Float2& Float2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}



// Division:
Float2 Float2::operator/(const Float2& other) const
{
	return Float2(x / other.x, y / other.y);
}
Float2& Float2::operator/=(const Float2& other)
{
	this->x /= other.x;
	this->y /= other.y;
	return *this;
}
Float2 Float2::operator/(float scalar) const
{
	return Float2(x / scalar, y / scalar);
}
Float2& Float2::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	return *this;
}
Float2 operator/(float scalar, const Float2& vector)
{
	return Float2(scalar / vector.x, scalar / vector.y);
}



// Comparison:
bool Float2::IsEpsilonEqual(const Float2& other) const
{
	return std::fabs(x - other.x) < mathf::epsilon && std::fabs(y - other.y) < mathf::epsilon;
}
bool Float2::operator==(const Float2& other) const
{
	return x == other.x && y == other.y;
}
bool Float2::operator!=(const Float2& other) const
{
	return !(*this == other);
}



// Friend functions:
Float2 operator*(float a, const Float2& b)
{
	return Float2(a * b.x, a * b.y);
}
Float2 operator*(const Float2& a, float b)
{
	return Float2(a.x * b, a.y * b);
}



// Logging:
std::string Float2::ToString() const
{
	std::ostringstream oss;
	oss << "(" << x << ", " << y << ")";
	return oss.str();
}
std::ostream& operator<<(std::ostream& os, const Float2& value)
{
	os << value.ToString();
	return os;
}



// Static members:
// Numbers:
Float2 Float2::zero	= Float2(0.0f);
Float2 Float2::one	= Float2(1.0f);
Float2 Float2::max	= Float2(mathf::max);
Float2 Float2::min	= Float2(mathf::min);

// Directions:
Float2 Float2::right	= Float2( 1.0f, 0.0f);
Float2 Float2::left		= Float2(-1.0f, 0.0f);
Float2 Float2::forward	= Float2( 0.0f, 1.0f);
Float2 Float2::back		= Float2( 0.0f,-1.0f);