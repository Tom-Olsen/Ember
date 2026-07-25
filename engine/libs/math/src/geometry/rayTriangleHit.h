#pragma once
#include "rayHit.h"



namespace emberMath
{
    class RayTriangleHit
    {
	private: // Members:
        RayHit m_rayHit;
        Float3 m_barycentricCoordinates;

	public: // Methods:
        // Constructors:
        RayTriangleHit();
        RayTriangleHit(float distance, const Float3& point, const Float3& normal, const Float3& barycentricCoordinates);

        // Getters:
        bool GetHit();
        float GetDistance();
        Float3 GetPoint();
        Float3 GetNormal();
        Float3 GetBarycentricCoordinates();
    };
}