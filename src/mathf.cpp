#include "mathf.h"
#include <math.h>
#include <iostream>
#include <sstream>
#include "logger.h"
constexpr float PI = 3.14159265358979323846f;



// Class/Struct independent functions:
namespace mathf
{
	float ToDegrees(float radians)
	{
		return radians * 180.0f / PI;
	}
	float ToRadians(float degrees)
	{
		return degrees * PI / 180.0f;
	}
}



// Int2:
// Constructors:
Int2::Int2() : x(0), y(0) {}
Int2::Int2(int xy) : x(xy), y(xy) {}
Int2::Int2(int x, int y) : x(x), y(y) {}
Int2::Int2(const Int2& xy) : x(xy.x), y(xy.y) {}

// Object math operations:

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



// Int3:
// Constructors:
Int3::Int3() : x(0), y(0), z(0) {}
Int3::Int3(int xyz) : x(xyz), y(xyz), z(xyz) {}
Int3::Int3(int x, int y) : x(x), y(y), z(0) {}
Int3::Int3(int x, int y, int z) : x(x), y(y), z(z) {}
Int3::Int3(const Int2& xy) : x(xy.x), y(xy.y), z(0) {}
Int3::Int3(const Int2& xy, int z) : x(xy.x), y(xy.y), z(z) {}
Int3::Int3(const Int3& xyz) : x(xyz.x), y(xyz.y), z(xyz.z) {}

// Object math operations:

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



// Uint3:
// Constructors:
Uint3::Uint3() : x(0), y(0), z(0) {}
Uint3::Uint3(uint32_t xyz) : x(xyz), y(xyz), z(xyz) {}
Uint3::Uint3(uint32_t x, uint32_t y, uint32_t z) : x(x), y(y), z(z) {}
Uint3::Uint3(const Uint3& xyz) : x(xyz.x), y(xyz.y), z(xyz.z) {}

// Object math operations:

// Static math operations:

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

// Comparison:
bool Uint3::operator==(const Uint3& other) const
{
	return x == other.x && y == other.y && z == other.z;
}
bool Uint3::operator!=(const Uint3& other) const
{
	return !(*this == other);
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
Uint3 Uint3::zero = Uint3(0);
Uint3 Uint3::one = Uint3(1);
Uint3 Uint3::right = Uint3(1, 0, 0);
Uint3 Uint3::up = Uint3(0, 1, 0);
Uint3 Uint3::forward = Uint3(0, 0, 1);



// Float2:
// Constructors:
Float2::Float2() : x(0), y(0) {}
Float2::Float2(float xy) : x(xy), y(xy) {}
Float2::Float2(float x, float y) : x(x), y(y) {}
Float2::Float2(const Float2& xy) : x(xy.x), y(xy.y) {}
Float2::Float2(const Float3& xy) : x(xy.x), y(xy.y) {}
Float2::Float2(const Float4& xy) : x(xy.x), y(xy.y) {}
Float2 Float2::Direction(float radians)
{
	return Float2(cos(radians), sin(radians));
}

// Object math operations:
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
	return mathf::ToDegrees(atan2(y, x));
}
float Float2::AngleRadians() const
{
	return atan2(y, x);
}
Float2 Float2::Normalize() const
{
	float length = Length();
	if (length <= epsilon)
		return Float2(0.0f);
	return Float2(x / length, y / length);
}
Float2 Float2::Rotate(float radians) const
{
	float c = cos(radians);
	float s = sin(radians);
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
	return Length2() <= epsilon * epsilon;
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
	if (lengths <= epsilon)
		return 0.0f;
	return mathf::ToDegrees(acos(Dot(a, b) / lengths));
}
float Float2::AngleRadians(const Float2& a, const Float2& b)
{
	float lengths = a.Length() * b.Length();
	if (lengths <= epsilon)
		return 0.0f;
	return acos(Dot(a, b) / lengths);
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

// Comparison:
bool Float2::IsEpsilonEqual(const Float2& other) const
{
	return std::fabs(x - other.x) < epsilon && std::fabs(y - other.y) < epsilon;
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
	float a = PI / 180.0f;
	return a * (*this);
}
Float2 Float2::ToRadians() const
{
	float a = 180.0f / PI;
	return a * (*this);
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



// Float3:
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

// Object math operations:
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



// Float4:
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
	float st = sin(theta);
	float ct = cos(theta);
	float sp = sin(phi);
	float cp = cos(phi);
	return Float4(st * cp, st * sp, ct, 0.0f);
}

// Object math operations:
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
	return Length2() <= epsilon * epsilon;
}

// Static math operations:


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

// Comparison:
bool Float4::IsEpsilonEqual(const Float4& other) const
{
	return std::fabs(x - other.x) < epsilon && std::fabs(y - other.y) < epsilon && std::fabs(z - other.z) < epsilon && std::fabs(w - other.w) < epsilon;
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
Float4 Float4::zero = Float4(0.0f);
Float4 Float4::one = Float4(1.0f);
Float4 Float4::right = Float4(1.0f, 0.0f, 0.0f, 0.0f);
Float4 Float4::left = Float4(-1.0f, 0.0f, 0.0f, 0.0f);
Float4 Float4::up = Float4(0.0f, 1.0f, 0.0f, 0.0f);
Float4 Float4::down = Float4(0.0f, -1.0f, 0.0f, 0.0f);
Float4 Float4::forward = Float4(0.0f, 0.0f, 1.0f, 0.0f);
Float4 Float4::backward = Float4(0.0f, 0.0f, -1.0f, 0.0f);
Float4 Float4::in = Float4(0.0f, 0.0f, 0.0f, 1.0f);
Float4 Float4::out = Float4(0.0f, 0.0f, 0.0f, -1.0f);



// Float3x3:
// Constructors:
Float3x3::Float3x3()
{
	for (uint32_t i = 0; i < 9; i++)
		data[i] = 0.0f;
}
Float3x3::Float3x3(float value)
{
	for (uint32_t i = 0; i < 9; i++)
		data[i] = value;
}
Float3x3::Float3x3(const float* array)
{
	for (uint32_t i = 0; i < 9; i++)
		data[i] = array[i];
}
Float3x3::Float3x3
(float xx, float xy, float xz,	// column 0
 float yx, float yy, float yz,	// column 1
 float zx, float zy, float zz)	// column 2
{
	data[0] = xx; data[3] = yx; data[6] = zx;
	data[1] = xy; data[4] = yy; data[7] = zy;
	data[2] = xz; data[5] = yz; data[8] = zz;
}
Float3x3::Float3x3(const Float3x3& other)
{
	for (uint32_t i = 0; i < 9; i++)
		data[i] = other.data[i];
}

// Static constructors:
Float3x3 Float3x3::Rows(const Float3& row0, const Float3& row1, const Float3& row2)
{
	return Float3x3
	(row0.x, row1.x, row2.x,
	 row0.y, row1.y, row2.y,
	 row0.z, row1.z, row2.z);
}
Float3x3 Float3x3::Rows
(float row0x, float row0y, float row0z,
 float row1x, float row1y, float row1z,
 float row2x, float row2y, float row2z)
{
	return Float3x3
	(row0x, row1x, row2x,
	 row0y, row1y, row2y,
	 row0z, row1z, row2z);
}
Float3x3 Float3x3::Columns(const Float3& column0, const Float3& column1, const Float3& column2)
{
	return Float3x3
	(column0.x, column0.y, column0.z,
	 column1.x, column1.y, column1.z,
	 column2.x, column2.y, column2.z);
}
Float3x3 Float3x3::Columns
(float column0x, float column0y, float column0z,
 float column1x, float column1y, float column1z,
 float column2x, float column2y, float column2z)
{
	return Float3x3
	(column0x, column0y, column0z,
	 column1x, column1y, column1z,
	 column2x, column2y, column2z);
}

// Object math operations:
Float3x3 Float3x3::Transpose() const
{
	return Float3x3
	(data[0], data[3], data[6],
	 data[1], data[4], data[7],
	 data[2], data[5], data[8]);
}
float Float3x3::Determinant() const
{
	return data[0] * (data[4] * data[8] - data[5] * data[7])
		 - data[1] * (data[3] * data[8] - data[5] * data[6])
		 + data[2] * (data[3] * data[7] - data[4] * data[6]);
}
Float3x3 Float3x3::Inverse() const
{
	float det = Determinant();
	if (det == 0.0f)
		return Float3x3::zero;
	float invDet = 1.0f / det;
	return Float3x3::Columns
	(
		(data[4] * data[8] - data[5] * data[7]) * invDet,
		(data[2] * data[7] - data[1] * data[8]) * invDet,
		(data[1] * data[5] - data[2] * data[4]) * invDet,

		(data[5] * data[6] - data[3] * data[8]) * invDet,
		(data[0] * data[8] - data[2] * data[6]) * invDet,
		(data[2] * data[3] - data[0] * data[5]) * invDet,

		(data[3] * data[7] - data[4] * data[6]) * invDet,
		(data[1] * data[6] - data[0] * data[7]) * invDet,
		(data[0] * data[4] - data[1] * data[3]) * invDet
	);
}
bool Float3x3::IsEpsilonZero() const
{
	for (uint32_t i = 0; i < 9; i++)
		if (std::fabs(data[i]) > epsilon)
			return false;
	return true;
}

// Static math operations:
Float3x3 Float3x3::RotateX(float radians)
{
	float c = cos(radians);
	float s = sin(radians);
	return Float3x3::Rows
	(1.0f, 0.0f, 0.0f,
	 0.0f,    c,   -s,
	 0.0f,    s,    c);
}
Float3x3 Float3x3::RotateY(float radians)
{
	float c = cos(radians);
	float s = sin(radians);
	return Float3x3::Rows
	(c, 0.0f,    s,
  0.0f, 1.0f, 0.0f,
	-s, 0.0f,    c);
}
Float3x3 Float3x3::RotateZ(float radians)
{
	float c = cos(radians);
	float s = sin(radians);
	return Float3x3::Rows
	(c,   -s, 0.0f,
     s,    c, 0.0f,
  0.0f, 0.0f, 1.0f);
}
Float3x3 Float3x3::Rotate(const Float3& axis, float radians)
{
	float c = cos(radians);
	float s = sin(radians);
	float t = 1.0f - c;
	Float3 normalizedAxis = axis.Normalize();
	float x = normalizedAxis.x;
	float y = normalizedAxis.y;
	float z = normalizedAxis.z;
	return Float3x3::Rows
	(x * x * t +     c, x * y * t - z * s, x * z * t + y * s,
	 y * x * t + z * s, y * y * t +     c, y * z * t - x * s,
	 z * x * t - y * s, z * y * t + x * s, z * z * t +     c);
}
Float3x3 Float3x3::Rotate(const Float3& eulerRadians, const Uint3& rotationOrder, CoordinateSystem rotationSystem)
{
	Float3x3 rot[3] = { RotateX(eulerRadians.x), RotateY(eulerRadians.y), RotateZ(eulerRadians.z) };
	if (rotationSystem == CoordinateSystem::local)
		return rot[rotationOrder.x] * rot[rotationOrder.y] * rot[rotationOrder.z];
	// (rotationSystem == CoordinateSystem::World)
		return rot[rotationOrder.z] * rot[rotationOrder.y] * rot[rotationOrder.x];
}
Float3x3 Float3x3::RotateFromTo(const Float3& from, const Float3& to)
{
	Float3 f = from.Normalize();
	Float3 t = to.Normalize();
	Float3 axis = Float3::Cross(f, t).Normalize();
	float radians = Float3::AngleRadians(f, t);
	return Rotate(axis, radians);
}
Float3x3 Float3x3::RotateThreeLeg(const Float3& forwardOld, const Float3& forwardNew, const Float3& upOld, const Float3& upNew)
{
	// Rotate forwardOld to forwardNew:
	Float3 axis = Float3::Cross(forwardOld, forwardNew);
	float radians = Float3::AngleRadians(forwardOld, forwardNew);
	Float3x3 rot0 = Rotate(axis, radians);

	// Compute missalignment angle between upNew and upOld rotated by rot0:
	Float3 upOldRotated = rot0 * upOld;
	Float3 planeNormal = Float3::Cross(upNew, forwardNew).Normalize();
	Float3 projection = Float3::VectorToPlaneProjection(upOldRotated, planeNormal);
	float angle = -Float3::AngleRadians(upOldRotated, projection);
	if (Float3::Dot(upNew, upOldRotated) < 0)
		angle += PI;

	// Rotate by angle around forwardNew:
	Float3x3 rot1 = Float3x3::Rotate(forwardNew, angle);

	// Combine rotations:
	return rot1 * rot0;
}

// Access:
float& Float3x3::operator[](int index)
{
	if (index >= 0 && index < 9)
		return data[index];
	throw std::out_of_range("Float4x4 index out of range.");
}
float Float3x3::operator[](int index) const
{
	if (index >= 0 && index < 9)
		return data[index];
	throw std::out_of_range("Float4x4 index out of range.");
}
float& Float3x3::operator[](const Index2& index)
{
	if (index.i >= 0 && index.i < 3 && index.j >= 0 && index.j < 3)
		return data[index.i + 3 * index.j];
	throw std::out_of_range("Float3x3 index out of range.");
}
float Float3x3::operator[](const Index2& index) const
{
	if (index.i >= 0 && index.i < 3 && index.j >= 0 && index.j < 3)
		return data[index.i + 3 * index.j];
	throw std::out_of_range("Float3x3 index out of range.");
}
Float3 Float3x3::GetRow(int index) const
{
	if (index >= 0 && index < 3)
		return Float3(data[index], data[index + 3], data[index + 6]);
	throw std::out_of_range("Float3x3 row index out of range.");
}
Float3 Float3x3::GetColumn(int index) const
{
	if (index >= 0 && index < 3)
		return Float3(data[3 * index], data[3 * index + 1], data[3 * index + 2]);
	throw std::out_of_range("Float3x3 column index out of range.");
}

// Assignment:
Float3x3& Float3x3::operator=(const Float3x3& other)
{
	if (this != &other)
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] = other.data[i];
	}
	return *this;
}
Float3x3& Float3x3::operator=(Float3x3&& other) noexcept
{
	if (this != &other)
	{
		for (uint32_t i = 0; i < 9; i++)
			data[i] = other.data[i];
	}
	return *this;
}

// Addition:
Float3x3 Float3x3::operator+(const Float3x3& other) const
{
	Float3x3 result;
	for (uint32_t i = 0; i < 9; i++)
		result.data[i] = data[i] + other.data[i];
	return result;
}
Float3x3& Float3x3::operator+=(const Float3x3& other)
{
	for (uint32_t i = 0; i < 9; i++)
		data[i] += other.data[i];
	return *this;
}

// Substraction:
Float3x3 Float3x3::operator-(const Float3x3& other) const
{
	Float3x3 result;
	for (uint32_t i = 0; i < 9; i++)
		result.data[i] = data[i] - other.data[i];
	return result;
}
Float3x3& Float3x3::operator-=(const Float3x3& other)
{
	for (uint32_t i = 0; i < 9; i++)
		data[i] -= other.data[i];
	return *this;
}
Float3x3 Float3x3::operator-() const
{
	return Float3x3
	(-data[0], -data[1], -data[2],
	 -data[3], -data[4], -data[5],
	 -data[6], -data[7], -data[8]);
}


// Multiplication:
Float3x3 Float3x3::operator*(const Float3x3& other) const
{
	Float3x3 result;
	for (uint32_t i = 0; i < 3; i++)
		for (uint32_t j = 0; j < 3; j++)
			for (uint32_t k = 0; k < 3; k++)
				result[{i, j}] += (*this)[{i, k}] * other[{k, j}];
	return result;
}
Float3x3& Float3x3::operator*=(const Float3x3& other)
{
	Float3x3 result;
	for (uint32_t i = 0; i < 3; i++)
		for (uint32_t j = 0; j < 3; j++)
			for (uint32_t k = 0; k < 3; k++)
				result[{i, j}] += (*this)[{i, k}] * other[{k, j}];
	*this = result;
	return *this;
}
Float3x3 Float3x3::operator*(float scalar) const
{
	Float3x3 result;
	for (uint32_t i = 0; i < 9; i++)
		result.data[i] = data[i] * scalar;
	return result;
}
Float3x3& Float3x3::operator*=(float scalar)
{
	for (uint32_t i = 0; i < 9; i++)
		data[i] *= scalar;
	return *this;
}
Float3 Float3x3::operator*(const Float3& vector) const
{
	return Float3
	(data[0] * vector.x + data[3] * vector.y + data[6] * vector.z,
	 data[1] * vector.x + data[4] * vector.y + data[7] * vector.z,
	 data[2] * vector.x + data[5] * vector.y + data[8] * vector.z);
}

// Division:
Float3x3 Float3x3::operator/(float scalar) const
{
	Float3x3 result;
	for (uint32_t i = 0; i < 9; i++)
		result.data[i] = data[i] / scalar;
	return result;
}
Float3x3& Float3x3::operator/=(float scalar)
{
	for (uint32_t i = 0; i < 9; i++)
		data[i] /= scalar;
	return *this;
}

// Comparison:
bool Float3x3::IsEpsilonEqual(const Float3x3& other) const
{
	for (uint32_t i = 0; i < 9; i++)
		if (std::fabs(data[i] - other.data[i]) > epsilon)
			return false;
	return true;
}
bool Float3x3::operator==(const Float3x3& other) const
{
	for (uint32_t i = 0; i < 9; i++)
		if (data[i] != other.data[i])
			return false;
	return true;
}
bool Float3x3::operator!=(const Float3x3& other) const
{
	return !(*this == other);
}

// Friend functions:
Float3x3 operator*(const Float3x3& a, float b)
{
	Float3x3 result;
	for (uint32_t i = 0; i < 9; i++)
		result.data[i] = a.data[i] * b;
	return result;
}
Float3x3 operator*(float a, const Float3x3& b)
{
	Float3x3 result;
	for (uint32_t i = 0; i < 9; i++)
		result.data[i] = a * b.data[i];
	return result;
}
Float3x3 operator/(const Float3x3& a, float b)
{
	Float3x3 result;
	for (uint32_t i = 0; i < 9; i++)
		result.data[i] = a.data[i] / b;
	return result;
}

// Logging:
std::string Float3x3::ToString() const
{
	std::ostringstream oss;
	oss <<  "("  << data[0] << ", " << data[3] << ", " << data[6];
	oss << " | " << data[1] << ", " << data[4] << ", " << data[7];
	oss << " | " << data[2] << ", " << data[5] << ", " << data[8] << ")";
	return oss.str();
}
std::ostream& operator<<(std::ostream& os, const Float3x3& value)
{
	os << value.ToString();
	return os;
}

// Static members:
Float3x3 Float3x3::zero = Float3x3(0.0f);
Float3x3 Float3x3::identity = Float3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);



// Float4x4:
// Constructors:
Float4x4::Float4x4()
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] = 0.0f;
}
Float4x4::Float4x4(float value)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] = value;
}
Float4x4::Float4x4(const float* array)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] = array[i];
}
Float4x4::Float4x4
(float xx, float xy, float xz, float xw,	// column 0
 float yx, float yy, float yz, float yw,	// column 1
 float zx, float zy, float zz, float zw,	// column 2
 float wx, float wy, float wz, float ww)	// column 3
{
	data[0] = xx; data[4] = yx; data[ 8] = zx; data[12] = wx;
	data[1] = xy; data[5] = yy; data[ 9] = zy; data[13] = wy;
	data[2] = xz; data[6] = yz; data[10] = zz; data[14] = wz;
	data[3] = xw; data[7] = yw; data[11] = zw; data[15] = ww;
}
Float4x4::Float4x4(const Float3x3& other)
{
	data[0] = other[0]; data[4] = other[3]; data[ 8] = other[6]; data[12] = 0.0f;
	data[1] = other[1]; data[5] = other[4]; data[ 9] = other[7]; data[13] = 0.0f;
	data[2] = other[2]; data[6] = other[5]; data[10] = other[8]; data[14] = 0.0f;
	data[3] =     0.0f; data[7] =     0.0f; data[11] =     0.0f; data[15] = 1.0f;

}
Float4x4::Float4x4(const Float4x4& other)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] = other.data[i];
}

// Static constructors:
Float4x4 Float4x4::Rows(const Float4& row0, const Float4& row1, const Float4& row2, const Float4& row3)
{
	return Float4x4
	(row0.x, row1.x, row2.x, row3.x,
	 row0.y, row1.y, row2.y, row3.y,
	 row0.z, row1.z, row2.z, row3.z,
	 row0.w, row1.w, row2.w, row3.w);
}
Float4x4 Float4x4::Rows
(float row0x, float row0y, float row0z, float row0w,
 float row1x, float row1y, float row1z, float row1w,
 float row2x, float row2y, float row2z, float row2w,
 float row3x, float row3y, float row3z, float row3w)
{
	return Float4x4
	(row0x, row1x, row2x, row3x,
	 row0y, row1y, row2y, row3y,
	 row0z, row1z, row2z, row3z,
	 row0w, row1w, row2w, row3w);
}
Float4x4 Float4x4::Columns(const Float4& column0, const Float4& column1, const Float4& column2, const Float4& column3)
{
	return Float4x4
	(column0.x, column0.y, column0.z, column0.w,
	 column1.x, column1.y, column1.z, column1.w,
	 column2.x, column2.y, column2.z, column2.w,
	 column3.x, column3.y, column3.z, column3.w);
}
Float4x4 Float4x4::Columns
(float column0x, float column0y, float column0z, float column0w,
 float column1x, float column1y, float column1z, float column1w,
 float column2x, float column2y, float column2z, float column2w,
 float column3x, float column3y, float column3z, float column3w)
{
	return Float4x4
	(column0x, column0y, column0z, column0w,
	 column1x, column1y, column1z, column1w,
	 column2x, column2y, column2z, column2w,
	 column3x, column3y, column3z, column3w);
}

// Object math operations:
Float4x4 Float4x4::Transpose()
{
	return Float4x4
	(data[0], data[4], data[ 8], data[12],
	 data[1], data[5], data[ 9], data[13],
	 data[2], data[6], data[10], data[14],
	 data[3], data[7], data[11], data[15]);
}
float Float4x4::Determinant() const
{
	return data[0] * (data[5] * (data[10] * data[15] - data[11] * data[14]) - data[6] * (data[9] * data[15] - data[11] * data[13]) + data[7] * (data[9] * data[14] - data[10] * data[13]))
		 - data[1] * (data[4] * (data[10] * data[15] - data[11] * data[14]) - data[6] * (data[8] * data[15] - data[11] * data[12]) + data[7] * (data[8] * data[14] - data[10] * data[12]))
		 + data[2] * (data[4] * (data[ 9] * data[15] - data[11] * data[13]) - data[5] * (data[8] * data[15] - data[11] * data[12]) + data[7] * (data[8] * data[13] - data[ 9] * data[12]))
		 - data[3] * (data[4] * (data[ 9] * data[14] - data[10] * data[13]) - data[5] * (data[8] * data[14] - data[10] * data[12]) + data[6] * (data[8] * data[13] - data[ 9] * data[12]));
}
Float4x4 Float4x4::Inverse() const
{
	float det = Determinant();
	if (det == 0.0f)
		return Float4x4::zero;
	float invDet = 1.0f / det;
	return Float4x4::Columns
	(// column 0:
		 (data[5] * (data[10] * data[15] - data[11] * data[14]) - data[6] * (data[9] * data[15] - data[11] * data[13]) + data[7] * (data[9] * data[14] - data[10] * data[13])) * invDet,
		-(data[1] * (data[10] * data[15] - data[11] * data[14]) - data[2] * (data[9] * data[15] - data[11] * data[13]) + data[3] * (data[9] * data[14] - data[10] * data[13])) * invDet,
		 (data[1] * (data[ 6] * data[15] - data[ 7] * data[14]) - data[2] * (data[5] * data[15] - data[ 7] * data[13]) + data[3] * (data[5] * data[14] - data[ 6] * data[13])) * invDet,
		-(data[1] * (data[ 6] * data[11] - data[ 7] * data[10]) - data[2] * (data[5] * data[11] - data[ 7] * data[ 9]) + data[3] * (data[5] * data[10] - data[ 6] * data[ 9])) * invDet,
	 // column 1:
		-(data[4] * (data[10] * data[15] - data[11] * data[14]) - data[6] * (data[8] * data[15] - data[11] * data[12]) + data[7] * (data[8] * data[14] - data[10] * data[12])) * invDet,
		 (data[0] * (data[10] * data[15] - data[11] * data[14]) - data[2] * (data[8] * data[15] - data[11] * data[12]) + data[3] * (data[8] * data[14] - data[10] * data[12])) * invDet,
		-(data[0] * (data[ 6] * data[15] - data[ 7] * data[14]) - data[2] * (data[4] * data[15] - data[ 7] * data[12]) + data[3] * (data[4] * data[14] - data[ 6] * data[12])) * invDet,
		 (data[0] * (data[ 6] * data[11] - data[ 7] * data[10]) - data[2] * (data[4] * data[11] - data[ 7] * data[ 8]) + data[3] * (data[4] * data[10] - data[ 6] * data[ 8])) * invDet,
	 // column 2:
		 (data[4] * (data[ 9] * data[15] - data[11] * data[13]) - data[5] * (data[8] * data[15] - data[11] * data[12]) + data[7] * (data[8] * data[13] - data[ 9] * data[12])) * invDet,
		-(data[0] * (data[ 9] * data[15] - data[11] * data[13]) - data[1] * (data[8] * data[15] - data[11] * data[12]) + data[3] * (data[8] * data[13] - data[ 9] * data[12])) * invDet,
		 (data[0] * (data[ 5] * data[15] - data[ 7] * data[13]) - data[1] * (data[4] * data[15] - data[ 7] * data[12]) + data[3] * (data[4] * data[13] - data[ 5] * data[12])) * invDet,
		-(data[0] * (data[ 5] * data[11] - data[ 7] * data[ 9]) - data[1] * (data[4] * data[11] - data[ 7] * data[ 8]) + data[3] * (data[4] * data[ 9] - data[ 5] * data[ 8])) * invDet,
	 // column 3:
		-(data[4] * (data[ 9] * data[14] - data[10] * data[13]) - data[5] * (data[8] * data[14] - data[10] * data[12]) + data[6] * (data[8] * data[13] - data[ 9] * data[12])) * invDet,
		 (data[0] * (data[ 9] * data[14] - data[10] * data[13]) - data[1] * (data[8] * data[14] - data[10] * data[12]) + data[2] * (data[8] * data[13] - data[ 9] * data[12])) * invDet,
		-(data[0] * (data[ 5] * data[14] - data[ 6] * data[13]) - data[1] * (data[4] * data[14] - data[ 6] * data[12]) + data[2] * (data[4] * data[13] - data[ 5] * data[12])) * invDet,
		 (data[0] * (data[ 5] * data[10] - data[ 6] * data[ 9]) - data[1] * (data[4] * data[10] - data[ 6] * data[ 8]) + data[2] * (data[4] * data[ 9] - data[ 5] * data[ 8])) * invDet
	);
}
bool Float4x4::IsEpsilonZero() const
{
	for (uint32_t i = 0; i < 16; i++)
		if (std::fabs(data[i]) > epsilon)
			return false;
	return true;
}

// Static math operations:
Float4x4 Float4x4::RotateX(float radians)
{
	float c = cos(radians);
	float s = sin(radians);
	return Float4x4::Rows
	(1.0f, 0.0f, 0.0f, 0.0f,
	 0.0f,    c,   -s, 0.0f,
	 0.0f,    s,    c, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::RotateY(float radians)
{
	float c = cos(radians);
	float s = sin(radians);
	return Float4x4::Rows
	(   c, 0.0f,    s, 0.0f,
	 0.0f, 1.0f, 0.0f, 0.0f,
	   -s, 0.0f,    c, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::RotateZ(float radians)
{
	float c = cos(radians);
	float s = sin(radians);
	return Float4x4::Rows
	(   c,   -s, 0.0f, 0.0f,
	    s,    c, 0.0f, 0.0f,
	 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::Rotate(const Float3& axis, float radians)
{
	float c = cos(radians);
	float s = sin(radians);
	float t = 1.0f - c;
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	return Float4x4::Rows
	(x * x * t +     c, x * y * t - z * s, x * z * t + y * s, 0.0f,
	 y * x * t + z * s, y * y * t +     c, y * z * t - x * s, 0.0f,
	 z * x * t - y * s, z * y * t + x * s, z * z * t +     c, 0.0f,
	              0.0f,              0.0f,              0.0f, 1.0f);
}
Float4x4 Float4x4::Rotate(const Float3& eulerRadians, const Uint3& rotationOrder, CoordinateSystem rotationSystem)
{
	Float4x4 rot[3] = { RotateX(eulerRadians.x), RotateY(eulerRadians.y), RotateZ(eulerRadians.z) };
	if (rotationSystem == CoordinateSystem::local)
		return rot[rotationOrder.x] * rot[rotationOrder.y] * rot[rotationOrder.z];
	else if (rotationSystem == CoordinateSystem::world)
		return rot[rotationOrder.z] * rot[rotationOrder.y] * rot[rotationOrder.x];
	return identity;
}
Float4x4 Float4x4::RotateFromTo(const Float3& from, const Float3& to)
{
	Float3 axis = Float3::Cross(from, to);
	float radians = Float3::AngleRadians(from, to);
	return Rotate(axis, radians);
}
Float4x4 Float4x4::RotateThreeLeg(const Float3& forwardOld, const Float3& forwardNew, const Float3& upOld, const Float3& upNew)
{
	// Rotate forwardOld to forwardNew:
	Float3 axis = Float3::Cross(forwardOld, forwardNew);
	float radians = Float3::AngleRadians(forwardOld, forwardNew);
	Float3x3 rot0 = Float3x3::Rotate(axis, radians);

	// Compute missalignment angle between upNew and upOld rotated by rot0:
	Float3 upOldRotated = rot0 * upOld;
	Float3 planeNormal = Float3::Cross(upNew, forwardNew).Normalize();
	Float3 projection = Float3::VectorToPlaneProjection(upOldRotated, planeNormal);
	float angle = -Float3::AngleRadians(upOldRotated, projection);
	if (Float3::Dot(upNew, upOldRotated) < 0)
		angle += PI;

	// Rotate by angle around forwardNew:
	Float3x3 rot1 = Float3x3::Rotate(forwardNew, angle);

	// Combine rotations:
	return Float4x4(rot1 * rot0);
}
Float4x4 Float4x4::Translate(const Float3& translation)
{
	return Float4x4::Rows
	(1.0f, 0.0f, 0.0f, translation.x,
	 0.0f, 1.0f, 0.0f, translation.y,
	 0.0f, 0.0f, 1.0f, translation.z,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::Scale(const Float3& scale)
{
	return Float4x4::Rows
	(scale.x, 0.0f, 0.0f, 0.0f,
	 0.0f, scale.y, 0.0f, 0.0f,
	 0.0f, 0.0f, scale.z, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);
}
Float4x4 Float4x4::Scale(float scale)
{
	return Scale(Float3(scale));
}
Float4x4 Float4x4::Perspective(float fovRadians, float aspectRatio, float nearClip, float farClip)
{
	float tanHalfFov = std::tan(0.5f * fovRadians);
	float xx = 1.0f / (aspectRatio * tanHalfFov);
	float yy = -1.0f / tanHalfFov;
	float zz = (farClip + nearClip) / (nearClip - farClip);
	float wz = 2.0f * farClip * nearClip / (nearClip - farClip);

	return Float4x4::Rows
	(  xx, 0.0f,  0.0f, 0.0f,
	 0.0f,   yy,  0.0f, 0.0f,
	 0.0f, 0.0f,    zz,   wz,
	 0.0f, 0.0f, -1.0f, 0.0f);
}
Float4x4 Float4x4::Orthographic(float left, float right, float bottom, float top, float nearClip, float farClip)
{
	float xx = 2.0f / (right - left);
	float yy = 2.0f / (top - bottom);
	float zz = -2.0f / (farClip - nearClip);
	float wx = -(right + left) / (right - left);
	float wy = -(top + bottom) / (top - bottom);
	float wz = -(farClip + nearClip) / (farClip - nearClip);

	return Float4x4::Rows
	(  xx, 0.0f, 0.0f,   wx,
	 0.0f,   yy, 0.0f,   wy,
	 0.0f, 0.0f,   zz,   wz,
	 0.0f, 0.0f, 0.0f, 1.0f);
}

// Access:
float& Float4x4::operator[](int index)
{
	if (index >= 0 && index < 16)
		return data[index];
	throw std::out_of_range("Float4x4 index out of range.");
}
float Float4x4::operator[](int index) const
{
	if (index >= 0 && index < 16)
		return data[index];
	throw std::out_of_range("Float4x4 index out of range.");
}
float& Float4x4::operator[](const Index2& index)
{
	if (index.i >= 0 && index.i < 4 && index.j >= 0 && index.j < 4)
		return data[index.i + 4 * index.j];
	throw std::out_of_range("Float4x4 index out of range.");
}
float Float4x4::operator[](const Index2& index) const
{
	if (index.i >= 0 && index.i < 4 && index.j >= 0 && index.j < 4)
		return data[index.i + 4 * index.j];
	throw std::out_of_range("Float4x4 index out of range.");
}
Float4 Float4x4::GetRow(int index) const
{
	if (index >= 0 && index < 4)
		return Float4(data[index], data[index + 4], data[index + 8], data[index + 12]);
	throw std::out_of_range("Float4x4 row index out of range.");
}
Float4 Float4x4::GetColumn(int index) const
{
	if (index >= 0 && index < 4)
		return Float4(data[4 * index], data[4 * index + 1], data[4 * index + 2], data[4 * index + 3]);
	throw std::out_of_range("Float4x4 column index out of range.");
}

// Assignment:
Float4x4& Float4x4::operator=(const Float4x4& other)
{
	if (this != &other)
	{
		for (uint32_t i = 0; i < 16; i++)
			data[i] = other.data[i];
	}
	return *this;
}
Float4x4& Float4x4::operator=(Float4x4&& other) noexcept
{
	if (this != &other)
	{
		for (uint32_t i = 0; i < 16; i++)
			data[i] = other.data[i];
	}
	return *this;
}

// Addition:
Float4x4 Float4x4::operator+(const Float4x4& other) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result.data[i] = data[i] + other.data[i];
	return result;
}
Float4x4& Float4x4::operator+=(const Float4x4& other)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] += other.data[i];
	return *this;
}

// Substraction:
Float4x4 Float4x4::operator-(const Float4x4& other) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result.data[i] = data[i] - other.data[i];
	return result;
}
Float4x4& Float4x4::operator-=(const Float4x4& other)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] -= other.data[i];
	return *this;
}
Float4x4 Float4x4::operator-() const
{
	return Float4x4
	(-data[ 0], -data[ 1], -data[ 2], -data[ 3],
	 -data[ 4], -data[ 5], -data[ 6], -data[ 7],
	 -data[ 8], -data[ 9], -data[10], -data[11],
	 -data[12], -data[13], -data[14], -data[15]);
}

// Multiplication:
Float4x4 Float4x4::operator*(const Float4x4& other) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 4; i++)
		for (uint32_t j = 0; j < 4; j++)
			for (uint32_t k = 0; k < 4; k++)
				result[{i, j}] += (*this)[{i, k}] * other[{k, j}];
	return result;
}
Float4x4& Float4x4::operator*=(const Float4x4& other)
{
	Float4x4 result;
	for (uint32_t i = 0; i < 4; i++)
		for (uint32_t j = 0; j < 4; j++)
			for (uint32_t k = 0; k < 4; k++)
				result[{i, j}] += (*this)[{i, k}] * other[{k, j}];
	*this = result;
	return *this;
}
Float4x4 Float4x4::operator*(float scalar) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result.data[i] = data[i] * scalar;
	return result;
}
Float4x4& Float4x4::operator*=(float scalar)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] *= scalar;
	return *this;
}
Float4 Float4x4::operator*(const Float4& vector) const
{
	return Float4
	(data[0] * vector.x + data[4] * vector.y + data[ 8] * vector.z + data[12] * vector.w,
	 data[1] * vector.x + data[5] * vector.y + data[ 9] * vector.z + data[13] * vector.w,
	 data[2] * vector.x + data[6] * vector.y + data[10] * vector.z + data[14] * vector.w,
	 data[3] * vector.x + data[7] * vector.y + data[11] * vector.z + data[15] * vector.w);
}

// Division:
Float4x4 Float4x4::operator/(float scalar) const
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result.data[i] = data[i] / scalar;
	return result;
}
Float4x4& Float4x4::operator/=(float scalar)
{
	for (uint32_t i = 0; i < 16; i++)
		data[i] /= scalar;
	return *this;
}

// Comparison:
bool Float4x4::IsEpsilonEqual(const Float4x4& other) const
{
	for (uint32_t i = 0; i < 16; i++)
		if (std::fabs(data[i] - other.data[i]) > epsilon)
			return false;
	return true;
}
bool Float4x4::operator==(const Float4x4& other) const
{
	for (uint32_t i = 0; i < 16; i++)
		if (data[i] != other.data[i])
			return false;
	return true;
}
bool Float4x4::operator!=(const Float4x4& other) const
{
	return !(*this == other);
}

// Friend functions:
Float4x4 operator*(float a, const Float4x4& b)
{
	Float4x4 result;
	for (uint32_t i = 0; i < 16; i++)
		result.data[i] = a * b.data[i];
	return result;
}

// Logging:
std::string Float4x4::ToString() const
{
	std::ostringstream oss;
	oss <<  "("  << data[0] << ", " << data[4] << ", " << data[ 8] << ", " << data[12];
	oss << " | " << data[1] << ", " << data[5] << ", " << data[ 9] << ", " << data[13];
	oss << " | " << data[1] << ", " << data[6] << ", " << data[10] << ", " << data[14];
	oss << " | " << data[3] << ", " << data[7] << ", " << data[11] << ", " << data[15] << ")";
	return oss.str();
}
std::ostream& operator<<(std::ostream& os, const Float4x4& value)
{
	os << value.ToString();
	return os;
}

// Static members:
Float4x4 Float4x4::zero = Float4x4(0.0f);
Float4x4 Float4x4::identity = Float4x4
(1.0f, 0.0f, 0.0f, 0.0f,
 0.0f, 1.0f, 0.0f, 0.0f,
 0.0f, 0.0f, 1.0f, 0.0f,
 0.0f, 0.0f, 0.0f, 1.0f);