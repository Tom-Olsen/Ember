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
Float2 Float2::Direction(float radians)
{
	return Float2(mathf::Cos(radians), mathf::Sin(radians));
}



// Math operations:
Float2 Float2::Abs() const
{
	return Float2(std::fabs(x), std::fabs(y));
}
float Float2::Length2() const
{
	return x * x + y * y;
}
float Float2::Length() const
{
	return sqrt(Length2());
}
float Float2::AngleDegrees() const
{
	return mathf::ToDegrees(mathf::Atan2(y, x));
}
float Float2::AngleRadians() const
{
	return mathf::Atan2(y, x);
}
Float2 Float2::Normalize() const
{
	float length = Length();
	if (length <= s_epsilon)
		return Float2(0.0f);
	return Float2(x / length, y / length);
}
Float2 Float2::Rotate(float radians) const
{
	float c = mathf::Cos(radians);
	float s = mathf::Sin(radians);
	return Float2(x * c - y * s, x * s + y * c);
}
Float2 Float2::Rotate90() const
{
	return Float2(-y, x);
}
Float2 Float2::Rotate270() const
{
	return Float2(y, -x);
}
bool Float2::IsEpsilonZero() const
{
	return Length2() <= s_epsilon * s_epsilon;
}



// Static math operations:
float Float2::Dot(const Float2& a, const Float2& b)
{
	return a.x * b.x + a.y * b.y;
}
float Float2::Cross(const Float2& a, const Float2& b)
{
	return a.y * b.x - a.x * b.y;
}
float Float2::Distance2(const Float2& a, const Float2& b)
{
	return (a - b).Length2();
}
float Float2::Distance(const Float2& a, const Float2& b)
{
	return (a - b).Length();
}
float Float2::AngleDegrees(const Float2& a, const Float2& b)
{
	float lengths = a.Length() * b.Length();
	if (lengths <= s_epsilon)
		return 0.0f;
	return mathf::ToDegrees(mathf::Acos(Dot(a, b) / lengths));
}
float Float2::AngleRadians(const Float2& a, const Float2& b)
{
	float lengths = a.Length() * b.Length();
	if (lengths <= s_epsilon)
		return 0.0f;
	return mathf::Acos(Dot(a, b) / lengths);
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
Float2 Float2::operator*(float scalar) const
{
	return Float2(x * scalar, y * scalar);
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
	return std::fabs(x - other.x) < s_epsilon && std::fabs(y - other.y) < s_epsilon;
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

// Conversion:
Float2 Float2::ToDegrees() const
{
	return mathf::RAD2DEG * (*this);
}
Float2 Float2::ToRadians() const
{
	return mathf::DEG2RAD * (*this);
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
Float2 Float2::zero = Float2(0.0f);
Float2 Float2::one = Float2(1.0f);
Float2 Float2::right = Float2(1.0f, 0.0f);
Float2 Float2::left = Float2(-1.0f, 0.0f);
Float2 Float2::up = Float2(0.0f, 1.0f);
Float2 Float2::down = Float2(0.0f, -1.0f);