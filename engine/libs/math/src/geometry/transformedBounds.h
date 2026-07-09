#pragma once
#include "bounds.h"
#include "float4x4.h"



namespace emberMath
{
    struct TransformedBounds
    {
	public: // Members:
        Bounds localBounds;

	private: // Members:
        Float4x4 m_localToWorld;
        Float4x4 m_worldToLocal;

    public: // Methods:
        // Constructors:
		TransformedBounds();
		TransformedBounds(const Bounds& bounds);
		TransformedBounds(const Bounds& bounds, Float4x4 localToWorld);

        // Getters:
        Float4x4 GetLocalToWorldMatrix() const;
        Float4x4 GetWorldToLocalMatrix() const;
		Float4x4 GetLocalToWorldNormalMatrix() const;
		Float4x4 GetWorldToLocalNormalMatrix() const;
		std::array<Float3, 8> GetCorners() const;
		Bounds GetWorldBounds() const;

        // Setters:
        void SetLocalToWorldMatrix(const Float4x4& matrix);
        void SetWorldToLocalMatrix(const Float4x4& matrix);

		// Methods:
		Float3 ClosestPoint(const Float3& point) const; // point outside => closest point on surface. point inside => point.
		bool Contains(const Float3& point) const;
		void Encapsulate(const Float3& point);
		void Encapsulate(const Bounds& bounds);
		std::optional<Float3> IntersectRay(const Ray& ray) const;

		// Equallity:
		bool operator==(const TransformedBounds& other) const;	
		bool operator!=(const TransformedBounds& other) const;
    };
}