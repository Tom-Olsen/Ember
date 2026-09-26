#include "eulerDegrees.h"
#include "eulerRadians.h"
#include "mathConstants.h"
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Constructors:
	EulerDegrees::EulerDegrees() : value(Float3::zero) {}
	EulerDegrees::EulerDegrees(const Float3& value) : value(value) {}
	EulerDegrees::EulerDegrees(float x, float y, float z) : value(x, y, z) {}
	EulerDegrees::EulerDegrees(Degrees x, Degrees y, Degrees z) : value(x.value, y.value, z.value) {}
	EulerDegrees::EulerDegrees(Radians x, Radians y, Radians z) : value(x.ToDegrees().value, y.ToDegrees().value, z.ToDegrees().value) {}



	// Conversions:
	EulerRadians EulerDegrees::ToRadians() const
	{
		return EulerRadians(math::deg2rad * value);
	}



	// Getters:
	Float3 EulerDegrees::GetValue() const
	{
		return value;
	}
	Degrees EulerDegrees::GetX() const
	{
		return Degrees(value.x);
	}
	Degrees EulerDegrees::GetY() const
	{
		return Degrees(value.y);
	}
	Degrees EulerDegrees::GetZ() const
	{
		return Degrees(value.z);
	}



	// Comparison:
	bool EulerDegrees::IsEpsilonEqual(const EulerDegrees& other, float absEpsilon, float relEpsilon) const
	{
		return value.IsEpsilonEqual(other.value, absEpsilon, relEpsilon);
	}
	bool EulerDegrees::operator==(const EulerDegrees& other) const
	{
		return value == other.value;
	}
	bool EulerDegrees::operator!=(const EulerDegrees& other) const
	{
		return value != other.value;
	}



	// Logging:
	std::string EulerDegrees::ToString() const
	{
		std::ostringstream oss;
		oss << value << " deg";
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const EulerDegrees& eulerDegrees)
	{
		os << eulerDegrees.ToString();
		return os;
	}
}