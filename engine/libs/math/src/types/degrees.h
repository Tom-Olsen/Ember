#pragma once
#include "mathConstants.h"
#include <iosfwd>
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct Radians;



	struct Degrees
	{
	public: // Members:
		float value = 0.0f;

	public: // Methods:
		// Constructors:
		constexpr Degrees() = default;
		explicit constexpr Degrees(float value) : value(value) {}

		// Conversions:
		[[nodiscard]] Radians ToRadians() const;

		// Addition:
		Degrees operator+(const Degrees& other) const;
		Degrees& operator+=(const Degrees& other);

		// Subtraction:
		Degrees operator-(const Degrees& other) const;
		Degrees& operator-=(const Degrees& other);
		Degrees operator-() const;

		// Multiplication:
		Degrees operator*(float scalar) const;
		Degrees& operator*=(float scalar);

		// Division:
		Degrees operator/(float scalar) const;
		float operator/(const Degrees& other) const;
		Degrees& operator/=(float scalar);

		// Comparison:
		bool IsEpsilonEqual(const Degrees& other, float absEpsilon = math::absEpsilon, float relEpsilon = math::relEpsilon) const;
		bool operator==(const Degrees& other) const;
		bool operator!=(const Degrees& other) const;
		bool operator<(const Degrees& other) const;
		bool operator<=(const Degrees& other) const;
		bool operator>(const Degrees& other) const;
		bool operator>=(const Degrees& other) const;

		// Friend functions:
		friend Degrees operator*(float scalar, const Degrees& degrees);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Degrees& degrees);
	};
}