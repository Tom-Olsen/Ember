#include "float3.h"
#include "float2.h"
#include "float4.h"
#include "mathConstants.h"
#include "mathFunctions.h"
#include <stdexcept>
#include <sstream>



namespace emberMath
{
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
		float st = math::Sin(theta);
		float ct = math::Cos(theta);
		float sp = math::Sin(phi);
		float cp = math::Cos(phi);
		return Float3(st * cp, st * sp, ct);
	}



	// Math operations:
	float Float3::LengthSq() const
	{
		return x * x + y * y + z * z;
	}
	float Float3::Length() const
	{
		return sqrt(LengthSq());
	}
	float Float3::Theta() const
	{
		return math::Atan2(sqrt(x * x + y * y), z);
	}
	float Float3::Phi() const
	{
		return math::Atan2(y, x);
	}
	Float2 Float3::Angles() const
	{
		return Float2(Theta(), Phi());
	}
	Float3 Float3::Normalize() const
	{
		float length = Length();
		if (length <= math::epsilon)
			return Float3(0.0f);
		return Float3(x / length, y / length, z / length);
	}
	Float3 Float3::Rotate(float theta, float phi) const
	{
		float length = Length();
		phi += Phi();	// no need for clamping due to periodicity
		theta += Theta();
		if (theta > math::pi)
			theta = 2.0f * math::pi - theta;
		else if (theta < 0.0f)
			theta = -theta;
		return length * Float3::Direction(theta, phi);
	}
	bool Float3::IsEpsilonZero() const
	{
		return IsEpsilonEqual(Float3::zero);
	}



	// Static math operations:
	Float3 Float3::Abs(const Float3& a)
	{
		return Float3(math::Abs(a.x), math::Abs(a.y), math::Abs(a.z));
	}
	Float3 Float3::Round(const Float3& value, int decimals)
	{
		return Float3(math::Round(value.x, decimals), math::Round(value.y, decimals), math::Round(value.z, decimals));
	}
	Float3 Float3::Ceil(const Float3& value, int decimals)
	{
		return Float3(math::Ceil(value.x), math::Ceil(value.y), math::Ceil(value.z));
	}
	Float3 Float3::Floor(const Float3& value, int decimals)
	{
		return Float3(math::Floor(value.x), math::Floor(value.y), math::Floor(value.z));
	}
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
	float Float3::DistanceSq(const Float3& a, const Float3& b)
	{
		return (a - b).LengthSq();
	}
	float Float3::Distance(const Float3& a, const Float3& b)
	{
		return (a - b).Length();
	}
	float Float3::Angle(const Float3& a, const Float3& b)
	{
		float lengths = a.Length() * b.Length();
		if (lengths <= math::epsilon)
			return 0.0f;
		return math::Acos(math::Clamp(Dot(a, b) / lengths, -1.0f, 1.0f));
	}
	Float3 Float3::Min(const Float3& a, const Float3& b)
	{
		return Float3(math::Min(a.x, b.x), math::Min(a.y, b.y), math::Min(a.z, b.z));
	}
	Float3 Float3::Max(const Float3& a, const Float3& b)
	{
		return Float3(math::Max(a.x, b.x), math::Max(a.y, b.y), math::Max(a.z, b.z));
	}
	Float3 Float3::Clamp(const Float3& value, const Float3& min, const Float3& max)
	{
		return Float3(math::Clamp(value.x, min.x, max.x), math::Clamp(value.y, min.y, max.y), math::Clamp(value.z, min.z, max.z));
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
		return std::fabs(x - other.x) < math::epsilon && std::fabs(y - other.y) < math::epsilon && std::fabs(z - other.z) < math::epsilon;
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
	Float3 operator/(float a, const Float3& b)
	{
		return Float3(a / b.x, a / b.y, a / b.z);
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
	// Numbers;
	Float3 Float3::zero = Float3(0.0f);
	Float3 Float3::one = Float3(1.0f);
	Float3 Float3::maxValue = Float3(math::maxValue);
	Float3 Float3::minValue = Float3(math::minValue);

	// Directions:
	Float3 Float3::right = Float3(1.0f, 0.0f, 0.0f);
	Float3 Float3::left = Float3(-1.0f, 0.0f, 0.0f);
	Float3 Float3::forward = Float3(0.0f, 1.0f, 0.0f);
	Float3 Float3::back = Float3(0.0f, -1.0f, 0.0f);
	Float3 Float3::up = Float3(0.0f, 0.0f, 1.0f);
	Float3 Float3::down = Float3(0.0f, 0.0f, -1.0f);

	// Colors:
	Float3 Float3::white = Float3(1.0f, 1.0f, 1.0f);
	Float3 Float3::gray = Float3(0.5f, 0.5f, 0.5f);
	Float3 Float3::black = Float3(0.0f, 0.0f, 0.0f);
	Float3 Float3::red = Float3(1.0f, 0.0f, 0.0f);
	Float3 Float3::green = Float3(0.0f, 1.0f, 0.0f);
	Float3 Float3::blue = Float3(0.0f, 0.0f, 1.0f);
	Float3 Float3::yellow = Float3(1.0f, 1.0f, 0.0f);
	Float3 Float3::cyan = Float3(0.0f, 1.0f, 1.0f);
	Float3 Float3::magenta = Float3(1.0f, 0.0f, 1.0f);
}