#include "transformedBounds.h"
#include "float4.h"



namespace emberMath
{
	// Public methods:
	// Constructors:
	TransformedBounds::TransformedBounds()
	: localBounds()
	{
		m_localToWorld = Float4x4::identity;
		m_worldToLocal = Float4x4::identity;
	}
	TransformedBounds::TransformedBounds(const Bounds& bounds)
	: localBounds(bounds)
	{
		m_localToWorld = Float4x4::identity;
		m_worldToLocal = Float4x4::identity;
	}
	TransformedBounds::TransformedBounds(const Bounds& bounds, Float4x4 localToWorld)
	: localBounds(bounds)
	{
		SetLocalToWorldMatrix(localToWorld);
	}



	// Getters:
	Float4x4 TransformedBounds::GetLocalToWorldMatrix() const
	{
		return m_localToWorld;
	}
	Float4x4 TransformedBounds::GetWorldToLocalMatrix() const
	{
		return m_worldToLocal;
	}
	Float4x4 TransformedBounds::GetLocalToWorldNormalMatrix() const
	{
		return m_worldToLocal.Transpose();
	}
	Float4x4 TransformedBounds::GetWorldToLocalNormalMatrix() const
	{
		return m_localToWorld.Transpose();
	}
	std::array<Float3, 8> TransformedBounds::GetCorners() const
	{
		std::array<Float3, 8> corners = localBounds.GetCorners();
		for (Float3& corner : corners)
			corner = Float3(m_localToWorld * Float4(corner, 1.0f));
		return corners;
	}
	Bounds TransformedBounds::GetWorldBounds() const
	{
		std::array<Float3, 8> corners = GetCorners();
		return Bounds(corners.data());
	}



	// Setters:
	void TransformedBounds::SetLocalToWorldMatrix(const Float4x4& matrix)
	{
		m_localToWorld = matrix;
		m_worldToLocal = matrix.Inverse();
	}
	void TransformedBounds::SetWorldToLocalMatrix(const Float4x4& matrix)
	{
		m_worldToLocal = matrix;
		m_localToWorld = matrix.Inverse();
	}



	// Methods:
	Float3 TransformedBounds::ClosestPoint(const Float3& point) const
	{
		Float3 localPoint = Float3(m_worldToLocal * Float4(point, 1.0f));
		Float3 localClosestPoint = localBounds.ClosestPoint(localPoint);
		return Float3(m_localToWorld * Float4(localClosestPoint, 1.0f));
	}
	bool TransformedBounds::Contains(const Float3& point) const
	{
		Float3 localPoint = Float3(m_worldToLocal * Float4(point, 1.0f));
		return localBounds.Contains(localPoint);
	}
	void TransformedBounds::Encapsulate(const Float3& point)
	{
		Float3 localPoint = Float3(m_worldToLocal * Float4(point, 1.0f));
		localBounds.Encapsulate(localPoint);
	}
	void TransformedBounds::Encapsulate(const Bounds& bounds)
	{
		std::array<Float3, 8> corners = bounds.GetCorners();
		for (const Float3& corner : corners)
			Encapsulate(corner);
	}
	std::optional<Float3> TransformedBounds::IntersectRay(const Ray& ray) const
	{
		Float3 localOrigin = Float3(m_worldToLocal * Float4(ray.origin, 1.0f));
		Float3 localDirection = Float3(m_worldToLocal * Float4(ray.direction, 0.0f));
		if (localDirection.IsEpsilonZero())
			return std::nullopt;

		Ray localRay(localOrigin, localDirection);
		std::optional<Float3> localHit = localBounds.IntersectRay(localRay);
		if (!localHit.has_value())
			return std::nullopt;

		return Float3(m_localToWorld * Float4(localHit.value(), 1.0f));
	}



	// Equality:
	bool TransformedBounds::operator==(const TransformedBounds& other) const
	{
		return localBounds == other.localBounds
         && m_localToWorld == other.m_localToWorld;
	}
	bool TransformedBounds::operator!=(const TransformedBounds& other) const
	{
		return !((*this) == other);
	}
}