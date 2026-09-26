#include "degrees.h"
#include "mathConstants.h"
#include "mathFunctions.h"
#include "radians.h"
#include <cassert>
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Conversions:
	Radians Degrees::ToRadians() const
	{
		return Radians{value * math::deg2rad};
	}



	// Addition:
	Degrees Degrees::operator+(const Degrees& other) const
	{
		return Degrees(value + other.value);
	}
	Degrees& Degrees::operator+=(const Degrees& other)
	{
		value += other.value;
		return *this;
	}



	// Subtraction:
	Degrees Degrees::operator-(const Degrees& other) const
	{
		return Degrees(value - other.value);
	}
	Degrees& Degrees::operator-=(const Degrees& other)
	{
		value -= other.value;
		return *this;
	}
	Degrees Degrees::operator-() const
	{
		return Degrees(-value);
	}



	// Multiplication:
	Degrees Degrees::operator*(float scalar) const
	{
		return Degrees(value * scalar);
	}
	Degrees& Degrees::operator*=(float scalar)
	{
		value *= scalar;
		return *this;
	}



	// Division:
	Degrees Degrees::operator/(float scalar) const
	{
		assert(scalar != 0.0f);
		return Degrees(value / scalar);
	}
	float Degrees::operator/(const Degrees& other) const
	{
		assert(other.value != 0.0f);
		return value / other.value;
	}
	Degrees& Degrees::operator/=(float scalar)
	{
		assert(scalar != 0.0f);
		value /= scalar;
		return *this;
	}



	// Comparison:
	bool Degrees::IsEpsilonEqual(const Degrees& other, float absEpsilon, float relEpsilon) const
	{
		return math::IsEpsilonEqual(value, other.value, absEpsilon, relEpsilon);
	}
	bool Degrees::operator==(const Degrees& other) const
	{
		return value == other.value;
	}
	bool Degrees::operator!=(const Degrees& other) const
	{
		return value != other.value;
	}
	bool Degrees::operator<(const Degrees& other) const
	{
		return value < other.value;
	}
	bool Degrees::operator<=(const Degrees& other) const
	{
		return value <= other.value;
	}
	bool Degrees::operator>(const Degrees& other) const
	{
		return value > other.value;
	}
	bool Degrees::operator>=(const Degrees& other) const
	{
		return value >= other.value;
	}



	// Friend functions:
	Degrees operator*(float scalar, const Degrees& degrees)
	{
		return degrees * scalar;
	}



	// Logging:
	std::string Degrees::ToString() const
	{
		std::ostringstream oss;
		oss << value << " deg";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Degrees& degrees)
	{
		os << degrees.ToString();
		return os;
	}
}