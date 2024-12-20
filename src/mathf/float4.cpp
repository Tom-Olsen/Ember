#include "float4.h"
#include "float2.h"
#include "float3.h"
#include "mathf.h"
#include <stdexcept>
#include <sstream>



// Constructors:
Float4::Float4() : x(0), y(0), z(0), w(0) {}
Float4::Float4(float xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}
Float4::Float4(float x, float y) : x(x), y(y), z(0.0f), w(0.0f) {}
Float4::Float4(float x, float y, float z) : x(x), y(y), z(z), w(0.0f) {}
Float4::Float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
Float4::Float4(const Float2& xy) : x(xy.x), y(xy.y), z(0.0f), w(0.0f) {}
Float4::Float4(const Float2& xy, float z) : x(xy.x), y(xy.y), z(z), w(0.0f) {}
Float4::Float4(const Float2& xy, float z, float w) : x(xy.x), y(xy.y), z(z), w(w) {}
Float4::Float4(const Float2& xy, Float2 zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
Float4::Float4(const Float3& xyz) : x(xyz.x), y(xyz.y), z(xyz.z), w(0.0f) {}
Float4::Float4(const Float3& xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
Float4::Float4(const Float4& xyzw) : x(xyzw.x), y(xyzw.y), z(xyzw.z), w(xyzw.w) {}
Float4 Float4::Direction(float theta, float phi)
{
	float st = mathf::Sin(theta);
	float ct = mathf::Cos(theta);
	float sp = mathf::Sin(phi);
	float cp = mathf::Cos(phi);
	return Float4(st * cp, st * sp, ct, 0.0f);
}



// Math operations:
Float4 Float4::Abs() const
{
	return Float4(std::fabs(x), std::fabs(y), std::fabs(z), std::fabs(w));
}
float Float4::Length2() const
{
	return x * x + y * y + z * z + w * w;
}
float Float4::Length() const
{
	return sqrt(Length2());
}
bool Float4::IsEpsilonZero() const
{
	return Length2() <= s_epsilon * s_epsilon;
}



// Static math operations:
Float4 Float4::Min(const Float4& a, const Float4& b)
{
	return Float4(mathf::Min(a.x, b.x), mathf::Min(a.y, b.y), mathf::Min(a.z, b.z), mathf::Min(a.w, b.w));
}
Float4 Float4::Max(const Float4& a, const Float4& b)
{
	return Float4(mathf::Max(a.x, b.x), mathf::Max(a.y, b.y), mathf::Max(a.z, b.z), mathf::Max(a.w, b.w));
}
Float4 Float4::Clamp(const Float4& value, const Float4& min, const Float4& max)
{
	return Float4(mathf::Clamp(value.x, min.x, max.x), mathf::Clamp(value.y, min.y, max.y), mathf::Clamp(value.z, min.z, max.z), mathf::Clamp(value.w, min.w, max.w));
}



// Access:
float& Float4::operator[](int index)
{
	if (index == 0) return x;
	if (index == 1) return y;
	if (index == 2) return z;
	if (index == 3) return w;
	throw std::out_of_range("Float4 index out of range.");
}
float Float4::operator[](int index) const
{
	if (index == 0) return x;
	if (index == 1) return y;
	if (index == 2) return z;
	if (index == 3) return w;
	throw std::out_of_range("Float4 index out of range.");
}
Float3 Float4::xyz() const
{
	return Float3(x, y, z);
}



// Assignment:
Float4& Float4::operator=(const Float4& other)
{
	if (this != &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
	}
	return *this;
}
Float4& Float4::operator=(Float4&& other) noexcept
{
	if (this != &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
	}
	return *this;
}



// Addition:
Float4 Float4::operator+(const Float4& other) const
{
	return Float4(x + other.x, y + other.y, z + other.z, w + other.w);
}
Float4& Float4::operator+=(const Float4& other)
{
	this->x += other.x;
	this->y += other.y;
	this->z += other.z;
	this->w += other.w;
	return *this;
}



// Substraction:
Float4 Float4::operator-(const Float4& other) const
{
	return Float4(x - other.x, y - other.y, z - other.z, w - other.w);
}
Float4& Float4::operator-=(const Float4& other)
{
	this->x -= other.x;
	this->y -= other.y;
	this->z -= other.z;
	this->w -= other.w;
	return *this;
}
Float4 Float4::operator-() const
{
	return Float4(-x, -y, -z, -w);
}



// Multiplication:
Float4 Float4::operator*(const Float4& other) const
{
	return Float4(x * other.x, y * other.y, z * other.z, w * other.w);
}
Float4& Float4::operator*=(const Float4& other)
{
	this->x *= other.x;
	this->y *= other.y;
	this->z *= other.z;
	this->w *= other.w;
	return *this;
}
Float4 Float4::operator*(float scalar) const
{
	return Float4(x * scalar, y * scalar, z * scalar, w * scalar);
}
Float4& Float4::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}



// Division:
Float4 Float4::operator/(const Float4& other) const
{
	return Float4(x / other.x, y / other.y, z / other.z, w / other.w);
}
Float4& Float4::operator/=(const Float4& other)
{
	this->x /= other.x;
	this->y /= other.y;
	this->z /= other.z;
	this->w /= other.w;
	return *this;
}
Float4 Float4::operator/(float scalar) const
{
	return Float4(x / scalar, y / scalar, z / scalar, w / scalar);
}
Float4& Float4::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;
	return *this;
}
Float4 operator/(float scalar, const Float4& vector)
{
	return Float4(scalar / vector.x, scalar / vector.y, scalar / vector.z, scalar / vector.w);
}



// Comparison:
bool Float4::IsEpsilonEqual(const Float4& other) const
{
	return std::fabs(x - other.x) < s_epsilon && std::fabs(y - other.y) < s_epsilon && std::fabs(z - other.z) < s_epsilon && std::fabs(w - other.w) < s_epsilon;
}
bool Float4::operator==(const Float4& other) const
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}
bool Float4::operator!=(const Float4& other) const
{
	return !(*this == other);
}



// Friend functions:
Float4 operator*(float a, const Float4& b)
{
	return Float4(a * b.x, a * b.y, a * b.z, a * b.w);
}



// Logging:
std::string Float4::ToString() const
{
	std::ostringstream oss;
	oss << "(" << x << ", " << y << ", " << z << ", " << w << ")";
	return oss.str();
}
std::ostream& operator<<(std::ostream& os, const Float4& value)
{
	os << value.ToString();
	return os;
}



// Static members:
// Numbers;
Float4 Float4::zero		= Float4(0.0f);
Float4 Float4::one		= Float4(1.0f);

// Directions:
Float4 Float4::right	= Float4(1.0f, 0.0f, 0.0f, 0.0f);
Float4 Float4::left		= Float4(-1.0f, 0.0f, 0.0f, 0.0f);
Float4 Float4::up		= Float4(0.0f, 1.0f, 0.0f, 0.0f);
Float4 Float4::down		= Float4(0.0f, -1.0f, 0.0f, 0.0f);
Float4 Float4::forward	= Float4(0.0f, 0.0f, 1.0f, 0.0f);
Float4 Float4::backward	= Float4(0.0f, 0.0f, -1.0f, 0.0f);
Float4 Float4::in		= Float4(0.0f, 0.0f, 0.0f, 1.0f);
Float4 Float4::out		= Float4(0.0f, 0.0f, 0.0f, -1.0f);

// Colors:
Float4 Float4::white	= Float4(1.0f, 1.0f, 1.0f, 1.0f);
Float4 Float4::gray		= Float4(0.5f, 0.5f, 0.5f, 1.0f);
Float4 Float4::black	= Float4(0.0f, 0.0f, 0.0f, 1.0f);
Float4 Float4::red		= Float4(1.0f, 0.0f, 0.0f, 1.0f);
Float4 Float4::green	= Float4(0.0f, 1.0f, 0.0f, 1.0f);
Float4 Float4::blue		= Float4(0.0f, 0.0f, 1.0f, 1.0f);
Float4 Float4::yellow	= Float4(1.0f, 1.0f, 0.0f, 1.0f);
Float4 Float4::cyan		= Float4(0.0f, 1.0f, 1.0f, 1.0f);
Float4 Float4::magenta	= Float4(1.0f, 0.0f, 1.0f, 1.0f);