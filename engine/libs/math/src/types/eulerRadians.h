#pragma once
#include "degrees.h"
#include "float3.h"
#include "radians.h"
#include <iosfwd>
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct EulerDegrees;



	struct EulerRadians
	{
	public: // Members:
		Float3 value;

	public: // Methods:
		// Constructors:
		EulerRadians();
		explicit EulerRadians(const Float3& value);
		EulerRadians(float x, float y, float z);
		EulerRadians(Radians x, Radians y, Radians z);
		EulerRadians(Degrees x, Degrees y, Degrees z);

		// Conversions:
		[[nodiscard]] EulerDegrees ToDegrees() const;

		// Getters:
		Float3 GetValue() const;
		Radians GetX() const;
		Radians GetY() const;
		Radians GetZ() const;

		// Multiplication:
		EulerRadians operator*(float scalar) const;
		EulerRadians& operator*=(float scalar);

		// Division:
		EulerRadians operator/(float scalar) const;
		EulerRadians& operator/=(float scalar);

		// Friend functions:
		friend EulerRadians operator*(float scalar, const EulerRadians& angles);

		// Comparison:
		bool IsEpsilonEqual(const EulerRadians& other, float absEpsilon = math::absEpsilon, float relEpsilon = math::relEpsilon) const;
		bool operator==(const EulerRadians& other) const;
		bool operator!=(const EulerRadians& other) const;

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const EulerRadians& eulerRadians);
	};
}