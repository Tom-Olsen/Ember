#include "eulerRadians.h"
#include "eulerDegrees.h"
#include "float3x3.h"
#include "float4x4.h"
#include "mathConstants.h"
#include "mathFunctions.h"
#include <cassert>
#include <ostream>



namespace emberMath
{
	// Public methods:
	// Constructors:
	EulerRadians::EulerRadians() : value(Float3::zero) {}
	EulerRadians::EulerRadians(const Float3& value) : value(value) {}
	EulerRadians::EulerRadians(const Float3x3& rotationMatrix)
	{
		// Requires a pure rotation matrix and use R = RotY(y) * RotX(x) * RotZ(z).
		assert((rotationMatrix.Transpose() * rotationMatrix).IsEpsilonEqual(Float3x3::identity, math::absEpsilon, math::absEpsilon));
		assert(math::Abs(rotationMatrix.Determinant() - 1.0f) <= math::absEpsilon);

		float sinX = -rotationMatrix.data[7];
		float cosX = math::Sqrt(rotationMatrix.data[1] * rotationMatrix.data[1]
			+ rotationMatrix.data[4] * rotationMatrix.data[4]);
		Radians x = math::Atan2(sinX, cosX);
		Radians y;
		Radians z;
		if (cosX > math::absEpsilon)
		{
			y = math::Atan2(rotationMatrix.data[6], rotationMatrix.data[8]);
			z = math::Atan2(rotationMatrix.data[1], rotationMatrix.data[4]);
		}
		else
		{
			// At gimbal lock, choose z = 0 and recover the combined rotation in y.
			float sign = (sinX >= 0.0f) ? 1.0f : -1.0f;
			y = math::Atan2(sign * rotationMatrix.data[3], rotationMatrix.data[0]);
			z = Radians();
		}
		value = Float3(x.value, y.value, z.value);
	}
	EulerRadians::EulerRadians(const Float4x4& rotationMatrix)
		: EulerRadians(Float3x3(rotationMatrix))
	{
		// Requires a pure rotation matrix and use R = RotY(y) * RotX(x) * RotZ(z).
		assert(math::Abs(rotationMatrix.data[3]) <= math::absEpsilon);
		assert(math::Abs(rotationMatrix.data[7]) <= math::absEpsilon);
		assert(math::Abs(rotationMatrix.data[11]) <= math::absEpsilon);
		assert(math::Abs(rotationMatrix.data[12]) <= math::absEpsilon);
		assert(math::Abs(rotationMatrix.data[13]) <= math::absEpsilon);
		assert(math::Abs(rotationMatrix.data[14]) <= math::absEpsilon);
		assert(math::Abs(rotationMatrix.data[15] - 1.0f) <= math::absEpsilon);
	}
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



	// Multiplication:
	EulerRadians EulerRadians::operator*(float scalar) const
	{
		return EulerRadians(value * scalar);
	}
	EulerRadians& EulerRadians::operator*=(float scalar)
	{
		value *= scalar;
		return *this;
	}



	// Division:
	EulerRadians EulerRadians::operator/(float scalar) const
	{
		assert(scalar != 0.0f);
		return EulerRadians(value / scalar);
	}
	EulerRadians& EulerRadians::operator/=(float scalar)
	{
		assert(scalar != 0.0f);
		value /= scalar;
		return *this;
	}



	// Friend functions:
	EulerRadians operator*(float scalar, const EulerRadians& angles)
	{
		return angles * scalar;
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
	std::string EulerRadians::ToString(const char* format) const
	{
		return value.ToString(format) + " rad";
	}
	std::ostream& operator<<(std::ostream& os, const EulerRadians& eulerRadians)
	{
		os << eulerRadians.ToString();
		return os;
	}
}