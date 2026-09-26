#include "eulerRadians.h"
#include "eulerDegrees.h"
#include "mathConstants.h"
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Constructors:
	EulerRadians::EulerRadians() : value(Float3::zero) {}
	EulerRadians::EulerRadians(const Float3& value) : value(value) {}
	EulerRadians::EulerRadians(float x, float y, float z) : value(x, y, z) {}
	EulerRadians::EulerRadians(Radians x, Radians y, Radians z) : value(x.value, y.value, z.value) {}
	EulerRadians::EulerRadians(Degrees x, Degrees y, Degrees z) : value(x.ToRadians().value, y.ToRadians().value, z.ToRadians().value) {}



	// Conversions:
	EulerDegrees EulerRadians::ToDegrees() const
	{
		return EulerDegrees(math::rad2deg * value);
	}



	// Getters:
	Float3 EulerRadians::GetValue() const
	{
		return value;
	}
	Radians EulerRadians::GetX() const
	{
		return Radians(value.x);
	}
	Radians EulerRadians::GetY() const
	{
		return Radians(value.y);
	}
	Radians EulerRadians::GetZ() const
	{
		return Radians(value.z);
	}



	// Comparison:
	bool EulerRadians::IsEpsilonEqual(const EulerRadians& other, float absEpsilon, float relEpsilon) const
	{
		return value.IsEpsilonEqual(other.value, absEpsilon, relEpsilon);
	}
	bool EulerRadians::operator==(const EulerRadians& other) const
	{
		return value == other.value;
	}
	bool EulerRadians::operator!=(const EulerRadians& other) const
	{
		return value != other.value;
	}



	// Logging:
	std::string EulerRadians::ToString() const
	{
		std::ostringstream oss;
		oss << value << " rad";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const EulerRadians& eulerRadians)
	{
		os << eulerRadians.ToString();
		return os;
	}
}