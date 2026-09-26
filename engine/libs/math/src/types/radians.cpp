#include "radians.h"
#include "degrees.h"
#include "mathConstants.h"
#include "mathFunctions.h"
#include <cassert>
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Conversions:
	Degrees Radians::ToDegrees() const
	{
		return Degrees{value * math::rad2deg};
	}



	// Addition:
	Radians Radians::operator+(const Radians& other) const
	{
		return Radians(value + other.value);
	}
	Radians& Radians::operator+=(const Radians& other)
	{
		value += other.value;
		return *this;
	}



	// Subtraction:
	Radians Radians::operator-(const Radians& other) const
	{
		return Radians(value - other.value);
	}
	Radians& Radians::operator-=(const Radians& other)
	{
		value -= other.value;
		return *this;
	}
	Radians Radians::operator-() const
	{
		return Radians(-value);
	}



	// Multiplication:
	Radians Radians::operator*(float scalar) const
	{
		return Radians(value * scalar);
	}
	Radians& Radians::operator*=(float scalar)
	{
		value *= scalar;
		return *this;
	}



	// Division:
	Radians Radians::operator/(float scalar) const
	{
		assert(scalar != 0.0f);
		return Radians(value / scalar);
	}
	float Radians::operator/(const Radians& other) const
	{
		assert(other.value != 0.0f);
		return value / other.value;
	}
	Radians& Radians::operator/=(float scalar)
	{
		assert(scalar != 0.0f);
		value /= scalar;
		return *this;
	}



	// Comparison:
	bool Radians::IsEpsilonEqual(const Radians& other, float absEpsilon, float relEpsilon) const
	{
		return math::IsEpsilonEqual(value, other.value, absEpsilon, relEpsilon);
	}
	bool Radians::operator==(const Radians& other) const
	{
		return value == other.value;
	}
	bool Radians::operator!=(const Radians& other) const
	{
		return value != other.value;
	}
	bool Radians::operator<(const Radians& other) const
	{
		return value < other.value;
	}
	bool Radians::operator<=(const Radians& other) const
	{
		return value <= other.value;
	}
	bool Radians::operator>(const Radians& other) const
	{
		return value > other.value;
	}
	bool Radians::operator>=(const Radians& other) const
	{
		return value >= other.value;
	}



	// Friend functions:
	Radians operator*(float scalar, const Radians& radians)
	{
		return radians * scalar;
	}



	// Logging:
	std::string Radians::ToString() const
	{
		std::ostringstream oss;
		oss << value << " rad";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Radians& radians)
	{
		os << radians.ToString();
		return os;
	}
}