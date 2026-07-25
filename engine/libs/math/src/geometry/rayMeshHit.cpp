#include "rayMeshHit.h"
#include "logger.h"
#include <limits>



namespace emberMath
{
	// Public methods:
	// Constructors:
	RayMeshHit::RayMeshHit()
		: m_rayTriangleHit(), m_triangleIndex(std::numeric_limits<uint32_t>::max()) {}
	RayMeshHit::RayMeshHit(const RayTriangleHit& rayTriangleHit, uint32_t triangleIndex)
		: m_rayTriangleHit(rayTriangleHit), m_triangleIndex(triangleIndex) {}



	// Getters:
	bool RayMeshHit::GetHit()
	{
		return m_rayTriangleHit.GetHit();
	}
	float RayMeshHit::GetDistance()
	{
		return m_rayTriangleHit.GetDistance();
	}
	Float3 RayMeshHit::GetPoint()
	{
		return m_rayTriangleHit.GetPoint();
	}
	Float3 RayMeshHit::GetNormal()
	{
		return m_rayTriangleHit.GetNormal();
	}
	Float3 RayMeshHit::GetBarycentricCoordinates()
	{
		return m_rayTriangleHit.GetBarycentricCoordinates();
	}
	uint32_t RayMeshHit::GetTriangleIndex()
	{
		if (m_rayTriangleHit.GetHit() == false)
		{
			LOG_ERROR("Called RayMeshHit::GetTriangleIndex() on missed RayMeshHit. You fucked up.");
			return std::numeric_limits<uint32_t>::max();
		}
		return m_triangleIndex;
	}
}