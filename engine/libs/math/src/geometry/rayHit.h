#pragma once
#include "float3.h"



namespace emberMath
{
    class RayHit
    {
	private: // Members:
        bool m_hit;
        float m_distance;
        Float3 m_point;
        Float3 m_normal;

	public: // Methods:
        // Constructors:
        RayHit();
        RayHit(float distance, const Float3& point, const Float3& normal);

        // Getters:
        bool GetHit();
        float GetDistance();
        Float3 GetPoint();
        Float3 GetNormal();
    };
}