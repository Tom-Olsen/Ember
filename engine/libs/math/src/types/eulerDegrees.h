#pragma once
#include "degrees.h"
#include "float3.h"
#include "radians.h"
#include <iosfwd>
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct EulerRadians;



	struct EulerDegrees
	{
	public: // Members:
		Float3 value;

	public: // Methods:
		// Constructors:
		EulerDegrees();
		explicit EulerDegrees(const Float3& value);
		EulerDegrees(float x, float y, float z);
		EulerDegrees(Degrees x, Degrees y, Degrees z);
		EulerDegrees(Radians x, Radians y, Radians z);

		// Conversions:
		[[nodiscard]] EulerRadians ToRadians() const;

		// Getters:
		Float3 GetValue() const;
		Degrees GetX() const;
		Degrees GetY() const;
		Degrees GetZ() const;

		// Comparison:
		bool IsEpsilonEqual(const EulerDegrees& other, float absEpsilon = math::absEpsilon, float relEpsilon = math::relEpsilon) const;
		bool operator==(const EulerDegrees& other) const;
		bool operator!=(const EulerDegrees& other) const;

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const EulerDegrees& eulerDegrees);
	};
}