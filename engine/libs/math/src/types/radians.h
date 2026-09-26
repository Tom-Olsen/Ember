#pragma once
#include "mathConstants.h"
#include <iosfwd>
#include <string>



namespace emberMath
{
	// Forward declarations:
	struct Degrees;



	struct Radians
	{
	public: // Members:
		float value = 0.0f;

	public: // Methods:
		// Constructors:
		constexpr Radians() = default;
		explicit constexpr Radians(float value) : value(value) {}

		// Conversions:
		[[nodiscard]] Degrees ToDegrees() const;

		// Addition:
		Radians operator+(const Radians& other) const;
		Radians& operator+=(const Radians& other);

		// Subtraction:
		Radians operator-(const Radians& other) const;
		Radians& operator-=(const Radians& other);
		Radians operator-() const;

		// Multiplication:
		Radians operator*(float scalar) const;
		Radians& operator*=(float scalar);

		// Division:
		Radians operator/(float scalar) const;
		float operator/(const Radians& other) const;
		Radians& operator/=(float scalar);

		// Comparison:
		bool IsEpsilonEqual(const Radians& other, float absEpsilon = math::absEpsilon, float relEpsilon = math::relEpsilon) const;
		bool operator==(const Radians& other) const;
		bool operator!=(const Radians& other) const;
		bool operator<(const Radians& other) const;
		bool operator<=(const Radians& other) const;
		bool operator>(const Radians& other) const;
		bool operator>=(const Radians& other) const;

		// Friend functions:
		friend Radians operator*(float scalar, const Radians& radians);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Radians& radians);
	};
}