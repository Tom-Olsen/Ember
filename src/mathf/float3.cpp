#include "float3.h"
#include "float2.h"
#include "float4.h"
#include "mathf.h"
#include <stdexcept>
#include <sstream>



// Constructors:
Float3::Float3() : x(0), y(0), z(0) {}
Float3::Float3(float xyz) : x(xyz), y(xyz), z(xyz) {}
Float3::Float3(float x, float y) : x(x), y(y), z(0.0f) {}
Float3::Float3(float x, float y, float z) : x(x), y(y), z(z) {}
Float3::Float3(const Float2& xy) : x(xy.x), y(xy.y), z(0.0f) {}
Float3::Float3(const Float2& xy, float z) : x(xy.x), y(xy.y), z(z) {}
Float3::Float3(const Float3& xyz) : x(xyz.x), y(xyz.y), z(xyz.z) {}
Float3::Float3(const Float4& xyz) : x(xyz.x), y(xyz.y), z(xyz.z) {}
Float3 Float3::Direction(float theta, float phi)
{
	float st = sin(theta);
	float ct = cos(theta);
	float sp = sin(phi);
	float cp = cos(phi);
	return Float3(st * cp, st * sp, ct);
}



// Math operations:
float Float3::Length2() const
{
	return x * x + y * y + z * z;
}
float Float3::Length() const
{
	return sqrt(Length2());
}
float Float3::Theta() const
{
	return atan2(sqrt(x * x + y * y), z);
}
float Float3::Phi() const
{
	return atan2(y, x);
}
Float2 Float3::AnglesDegrees() const
{
	return Float2(Theta(), Phi()).ToDegrees();
}
Float2 Float3::AnglesRadians() const
{
	return Float2(Theta(), Phi());
}
Float3 Float3::Normalize() const
{
	float length = Length();
	if (length <= epsilon)
		return Float3(0.0f);
	return Float3(x / length, y / length, z / length);
}
Float3 Float3::Rotate(float theta, float phi) const
{
	float length = Length();
	phi += Phi();	// no need for clamping due to sin/cos periodicity
	theta += Theta();
	if (theta > PI)
		theta = 2.0f * PI - theta;
	else if (theta < 0.0f)
		theta = -theta;
	return length * Float3::Direction(theta, phi);
}
bool Float3::IsEpsilonZero() const
{
	return Length2() <= epsilon * epsilon;
}



// Static math operations:
float Float3::Dot(const Float3& a, const Float3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
Float3 Float3::Cross(const Float3& a, const Float3& b)
{
	return Float3
	(a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}
float Float3::VectorToPlaneDistance(const Float3& vec, const Float3& planeNormal)
{
	return Dot(vec, planeNormal) / Dot(planeNormal, planeNormal);
}
Float3 Float3::VectorToPlaneProjection(const Float3& vec, const Float3& planeNormal)
{
	return vec - VectorToPlaneDistance(vec, planeNormal) * planeNormal;
}
Float3 Float3::Reflect(const Float3& vec, const Float3& planeNormal)
{
	return vec - 2.0f * VectorToPlaneDistance(vec, planeNormal) * planeNormal.Normalize();
}
float Float3::Distance2(const Float3& a, const Float3& b)
{
	return (a - b).Length2();
}
float Float3::Distance(const Float3& a, const Float3& b)
{
	return (a - b).Length();
}
float Float3::AngleDegrees(const Float3& a, const Float3& b)
{
	float lengths = a.Length() * b.Length();
	if (lengths <= epsilon)
		return 0.0f;
	return mathf::ToDegrees(acos(Dot(a, b) / lengths));
}
float Float3::AngleRadians(const Float3& a, const Float3& b)
{
	float lengths = a.Length() * b.Length();
	if (lengths <= epsilon)
		return 0.0f;
	return acos(Dot(a, b) / lengths);
}



// Access:
float& Float3::operator[](int index)
{
	if (index == 0) return x;
	if (index == 1) return y;
	if (index == 2) return z;
	throw std::out_of_range("Float3 index out of range.");
}
float Float3::operator[](int index) const
{
	if (index == 0) return x;
	if (index == 1) return y;
	if (index == 2) return z;
	throw std::out_of_range("Float3 index out of range.");
}



// Assignment:
Float3& Float3::operator=(const Float3& other)
{
	if (this != &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}
	return *this;
}
Float3& Float3::operator=(Float3&& other) noexcept
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
Float3 Float3::operator+(const Float3& other) const
{
	return Float3(x + other.x, y + other.y, z + other.z);
}
Float3& Float3::operator+=(const Float3& other)
{
	this->x += other.x;
	this->y += other.y;
	this->z += other.z;
	return *this;
}



// Substraction:
Float3 Float3::operator-(const Float3& other) const
{
	return Float3(x - other.x, y - other.y, z - other.z);
}
Float3& Float3::operator-=(const Float3& other)
{
	this->x -= other.x;
	this->y -= other.y;
	this->z -= other.z;
	return *this;
}
Float3 Float3::operator-() const
{
	return Float3(-x, -y, -z);
}



// Multiplication:
Float3 Float3::operator*(const Float3& other) const
{
	return Float3(x * other.x, y * other.y, z * other.z);
}
Float3& Float3::operator*=(const Float3& other)
{
	this->x *= other.x;
	this->y *= other.y;
	this->z *= other.z;
	return *this;
}
Float3 Float3::operator*(float scalar) const
{
	return Float3(x * scalar, y * scalar, z * scalar);
}
Float3& Float3::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}



// Division:
Float3 Float3::operator/(const Float3& other) const
{
	return Float3(x / other.x, y / other.y, z / other.z);
}
Float3& Float3::operator/=(const Float3& other)
{
	this->x /= other.x;
	this->y /= other.y;
	this->z /= other.z;
	return *this;
}
Float3 Float3::operator/(float scalar) const
{
	return Float3(x / scalar, y / scalar, z / scalar);
}
Float3& Float3::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}



// Comparison:
bool Float3::IsEpsilonEqual(const Float3& other) const
{
	return std::fabs(x - other.x) < epsilon && std::fabs(y - other.y) < epsilon && std::fabs(z - other.z) < epsilon;
}
bool Float3::operator==(const Float3& other) const
{
	return x == other.x && y == other.y && z == other.z;
}
bool Float3::operator!=(const Float3& other) const
{
	return !(*this == other);
}



// Friend functions:
Float3 operator*(const Float3& a, float b)
{
	return Float3(a.x * b, a.y * b, a.z * b);
}
Float3 operator*(float a, const Float3& b)
{
	return Float3(a * b.x, a * b.y, a * b.z);
}
Float3 operator/(const Float3& a, float b)
{
	return Float3(a.x / b, a.y / b, a.z / b);
}



// Conversion:
Float3 Float3::ToDegrees() const
{
	float a = 180.0f / PI;
	return a * (*this);
}
Float3 Float3::ToRadians() const
{
	float a = PI / 180.0f;
	return a * (*this);
}



// Logging:
std::string Float3::ToString() const
{
	std::ostringstream oss;
	oss << "(" << x << ", " << y << ", " << z << ")";
	return oss.str();
}
std::ostream& operator<<(std::ostream& os, const Float3& value)
{
	os << value.ToString();
	return os;
}



// Static members:
Float3 Float3::zero = Float3(0.0f);
Float3 Float3::one = Float3(1.0f);
Float3 Float3::right = Float3(1.0f, 0.0f, 0.0f);
Float3 Float3::left = Float3(-1.0f, 0.0f, 0.0f);
Float3 Float3::up = Float3(0.0f, 1.0f, 0.0f);
Float3 Float3::down = Float3(0.0f, -1.0f, 0.0f);
Float3 Float3::forward = Float3(0.0f, 0.0f, 1.0f);
Float3 Float3::backward = Float3(0.0f, 0.0f, -1.0f);