#include "float4.h"
#include "float2.h"
#include "float3.h"
#include "mathConstants.h"
#include "mathFunctions.h"
#include <stdexcept>
#include <sstream>



namespace emberMath
{
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
		float st = math::Sin(theta);
		float ct = math::Cos(theta);
		float sp = math::Sin(phi);
		float cp = math::Cos(phi);
		return Float4(st * cp, st * sp, ct, 0.0f);
	}



	// Math operations:
	float Float4::LengthSq() const
	{
		return x * x + y * y + z * z + w * w;
	}
	float Float4::Length() const
	{
		return sqrt(LengthSq());
	}
	bool Float4::IsEpsilonZero() const
	{
		return IsEpsilonEqual(Float4::zero);
	}



	// Static math operations:
	Float4 Float4::Abs(const Float4& a)
	{
		return Float4(math::Abs(a.x), math::Abs(a.y), math::Abs(a.z), math::Abs(a.w));
	}
	Float4 Float4::Round(const Float4& value, int decimals)
	{
		return Float4(math::Round(value.x, decimals), math::Round(value.y, decimals), math::Round(value.z, decimals), math::Round(value.w, decimals));
	}
	Float4 Float4::Ceil(const Float4& value, int decimals)
	{
		return Float4(math::Ceil(value.x), math::Ceil(value.y), math::Ceil(value.z), math::Ceil(value.w));
	}
	Float4 Float4::Floor(const Float4& value, int decimals)
	{
		return Float4(math::Floor(value.x), math::Floor(value.y), math::Floor(value.z), math::Floor(value.w));
	}
	Float4 Float4::Min(const Float4& a, const Float4& b)
	{
		return Float4(math::Min(a.x, b.x), math::Min(a.y, b.y), math::Min(a.z, b.z), math::Min(a.w, b.w));
	}
	Float4 Float4::Max(const Float4& a, const Float4& b)
	{
		return Float4(math::Max(a.x, b.x), math::Max(a.y, b.y), math::Max(a.z, b.z), math::Max(a.w, b.w));
	}
	Float4 Float4::Clamp(const Float4& value, const Float4& min, const Float4& max)
	{
		return Float4(math::Clamp(value.x, min.x, max.x), math::Clamp(value.y, min.y, max.y), math::Clamp(value.z, min.z, max.z), math::Clamp(value.w, min.w, max.w));
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
		return std::fabs(x - other.x) < math::epsilon && std::fabs(y - other.y) < math::epsilon && std::fabs(z - other.z) < math::epsilon && std::fabs(w - other.w) < math::epsilon;
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
	Float4 operator*(const Float4& a, float b)
	{
		return Float4(a.x * b, a.y * b, a.z * b, a.w * b);
	}
	Float4 operator*(float a, const Float4& b)
	{
		return Float4(a * b.x, a * b.y, a * b.z, a * b.w);
	}
	Float4 operator/(const Float4& a, float b)
	{
		return Float4(a.x / b, a.y / b, a.z / b, a.w / b);
	}
	Float4 operator/(float a, const Float4& b)
	{
		return Float4(a / b.x, a / b.y, a / b.z, a / b.w);
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
	Float4 Float4::zero = Float4(0.0f);
	Float4 Float4::one = Float4(1.0f);
	Float4 Float4::maxValue = Float4(math::maxValue);
	Float4 Float4::minValue = Float4(math::minValue);

	// Directions:
	Float4 Float4::right = Float4(1.0f, 0.0f, 0.0f, 0.0f);
	Float4 Float4::left = Float4(-1.0f, 0.0f, 0.0f, 0.0f);
	Float4 Float4::forward = Float4(0.0f, 1.0f, 0.0f, 0.0f);
	Float4 Float4::back = Float4(0.0f, -1.0f, 0.0f, 0.0f);
	Float4 Float4::up = Float4(0.0f, 0.0f, 1.0f, 0.0f);
	Float4 Float4::down = Float4(0.0f, 0.0f, -1.0f, 0.0f);
	Float4 Float4::in = Float4(0.0f, 0.0f, 0.0f, 1.0f);
	Float4 Float4::out = Float4(0.0f, 0.0f, 0.0f, -1.0f);

	// Colors:
	Float4 Float4::white = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 Float4::gray = Float4(0.5f, 0.5f, 0.5f, 1.0f);
	Float4 Float4::black = Float4(0.0f, 0.0f, 0.0f, 1.0f);
	Float4 Float4::red = Float4(1.0f, 0.0f, 0.0f, 1.0f);
	Float4 Float4::green = Float4(0.0f, 1.0f, 0.0f, 1.0f);
	Float4 Float4::blue = Float4(0.0f, 0.0f, 1.0f, 1.0f);
	Float4 Float4::yellow = Float4(1.0f, 1.0f, 0.0f, 1.0f);
	Float4 Float4::cyan = Float4(0.0f, 1.0f, 1.0f, 1.0f);
	Float4 Float4::magenta = Float4(1.0f, 0.0f, 1.0f, 1.0f);
}