#pragma once
#include "float2.h"
#include "float3.h"
#include <array>
#include <vector>
#include <string>



namespace emberMath
{
	struct Bounds2d
	{
	public: // Members:
		Float2 center;
		Float2 extents;
		float depth;

	public: // Methods:
		Bounds2d();
		Bounds2d(const Float2& center, const Float2& extents, float depth = 0.0f);
		Bounds2d(const Bounds2d& bounds);
		Bounds2d(const Float2* const corners, float depth = 0.0f);
		Bounds2d(const std::vector<Float2>& points, float depth = 0.0f);

		// Getters:
		Float2 GetMin() const;
		Float2 GetMax() const;
		Float2 GetSize() const;
		Float3 GetCenter3D() const;
		float GetDiagonal() const;
		std::array<Float2, 4> GetCorners() const;

		// Setters:
		void SetMinMax(const Float2& min, const Float2& max);

		// Methods:
		Float2 ClosestPoint(const Float2& point) const;
		Float2 ClosestPointOnSurface(const Float2& point) const; // Math::TODO: needs unit test
		bool Contains(const Float2& point) const;
		void Encapsulate(const Float2& point);
		void Expand(float amount);
		void Expand(const Float2& amount);

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Bounds2d& bounds);
	};
}