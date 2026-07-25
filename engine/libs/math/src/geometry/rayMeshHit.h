#pragma once
#include "rayTriangleHit.h"
#include <stdint.h>



namespace emberMath
{
	class RayMeshHit
	{
	private: // Members:
		RayTriangleHit m_rayTriangleHit;
		uint32_t m_triangleIndex;

	public: // Methods:
		// Constructors:
		RayMeshHit();
		RayMeshHit(const RayTriangleHit& rayTriangleHit, uint32_t triangleIndex);

		// Getters:
		bool GetHit();
		float GetDistance();
		Float3 GetPoint();
		Float3 GetNormal();
		Float3 GetBarycentricCoordinates();
		uint32_t GetTriangleIndex();
	};
}