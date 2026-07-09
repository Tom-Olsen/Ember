#pragma once
#include "bounds.h"
#include "float3x3.h"
#include "float4x4.h"



namespace emberMath
{
    struct RotatedBounds
    {
	public: // Members:
        Bounds localBounds;

	private: // Members:
        Float3x3 m_rotation;

    public: // Methods:
        // Constructors:
		RotatedBounds();
		RotatedBounds(const Bounds& bounds);
		RotatedBounds(const Bounds& bounds, const Float3x3& rotation);

        // Getters:
        Float3x3 GetRotation3x3() const;
        Float4x4 GetRotation4x4() const;
		std::array<Float3, 8> GetCorners() const;
		Bounds GetWorldBounds() const;

        // Setters:
        void SetRotationMatrix(const Float3x3& rotation);

		// Methods:
		Float3 ClosestPoint(const Float3& point) const; // point outside => closest point on surface. point inside => point.
		bool Contains(const Float3& point) const;
		std::optional<Float3> IntersectRay(const Ray& ray) const;

		// Equallity:
		bool operator==(const RotatedBounds& other) const;	
		bool operator!=(const RotatedBounds& other) const;

	private: // Methods:
		Float3 LocalToWorldPoint(const Float3& point) const;
		Float3 WorldToLocalPoint(const Float3& point) const;
		Float3 LocalToWorldDirection(const Float3& direction) const;
		Float3 WorldToLocalDirection(const Float3& direction) const;
    };
}