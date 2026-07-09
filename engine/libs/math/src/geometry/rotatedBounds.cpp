#include "rotatedBounds.h"



namespace emberMath
{
	// Public methods:
	// Constructors:
	RotatedBounds::RotatedBounds()
	: localBounds()
	{
		m_rotation = Float3x3::identity;
	}
	RotatedBounds::RotatedBounds(const Bounds& bounds)
	: localBounds(bounds)
	{
		m_rotation = Float3x3::identity;
	}
	RotatedBounds::RotatedBounds(const Bounds& bounds, const Float3x3& rotation)
	: localBounds(bounds)
	{
		SetRotationMatrix(rotation);
	}



	// Getters:
	Float3x3 RotatedBounds::GetRotation3x3() const
	{
		return m_rotation;
	}
	Float4x4 RotatedBounds::GetRotation4x4() const
	{
		return Float4x4(m_rotation);
	}
	std::array<Float3, 8> RotatedBounds::GetCorners() const
	{
		std::array<Float3, 8> corners = localBounds.GetCorners();
		for (Float3& corner : corners)
			corner = LocalToWorldPoint(corner);
		return corners;
	}
	Bounds RotatedBounds::GetWorldBounds() const
	{
		std::array<Float3, 8> corners = GetCorners();
		return Bounds(corners.data());
	}



	// Setters:
	void RotatedBounds::SetRotationMatrix(const Float3x3& rotation)
	{
		m_rotation = rotation;
	}



	// Methods:
	Float3 RotatedBounds::ClosestPoint(const Float3& point) const
	{
		Float3 localPoint = WorldToLocalPoint(point);
		Float3 localClosestPoint = localBounds.ClosestPoint(localPoint);
		return LocalToWorldPoint(localClosestPoint);
	}
	bool RotatedBounds::Contains(const Float3& point) const
	{
		return localBounds.Contains(WorldToLocalPoint(point));
	}
	std::optional<Float3> RotatedBounds::IntersectRay(const Ray& ray) const
	{
		Float3 localOrigin = WorldToLocalPoint(ray.origin);
		Float3 localDirection = WorldToLocalDirection(ray.direction);
		if (localDirection.IsEpsilonZero())
			return std::nullopt;

		Ray localRay(localOrigin, localDirection);
		std::optional<Float3> localHit = localBounds.IntersectRay(localRay);
		if (!localHit.has_value())
			return std::nullopt;

		return LocalToWorldPoint(localHit.value());
	}



	// Equality:
	bool RotatedBounds::operator==(const RotatedBounds& other) const
	{
		return localBounds == other.localBounds
			&& m_rotation == other.m_rotation;
	}
	bool RotatedBounds::operator!=(const RotatedBounds& other) const
	{
		return !((*this) == other);
	}



	// Private methods:
	Float3 RotatedBounds::LocalToWorldPoint(const Float3& point) const
	{
		return localBounds.center + LocalToWorldDirection(point - localBounds.center);
	}
	Float3 RotatedBounds::WorldToLocalPoint(const Float3& point) const
	{
		return localBounds.center + WorldToLocalDirection(point - localBounds.center);
	}
	Float3 RotatedBounds::LocalToWorldDirection(const Float3& direction) const
	{
		return m_rotation * direction;
	}
	Float3 RotatedBounds::WorldToLocalDirection(const Float3& direction) const
	{
		return m_rotation.Inverse() * direction;
	}
}