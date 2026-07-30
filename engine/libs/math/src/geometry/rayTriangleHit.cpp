#include "rayTriangleHit.h"
#include "logger.h"



namespace emberMath
{
    // Public methods:
    // Constructors:
    RayTriangleHit::RayTriangleHit()
        : m_rayHit(), m_barycentricCoordinates(Float3::zero) {}
    RayTriangleHit::RayTriangleHit(float distance, const Float3& point, const Float3& normal, const Float3& barycentricCoordinates)
        : m_rayHit(distance, point, normal), m_barycentricCoordinates(barycentricCoordinates) {}



    // Getters:
    bool RayTriangleHit::GetHit()
    {
        return m_rayHit.GetHit();
    }
    float RayTriangleHit::GetDistance()
    {
        return m_rayHit.GetDistance();
    }
    Float3 RayTriangleHit::GetPoint()
    {
        return m_rayHit.GetPoint();
    }
    Float3 RayTriangleHit::GetNormal()
    {
        return m_rayHit.GetNormal();
    }
    Float3 RayTriangleHit::GetBarycentricCoordinates()
    {
        if (m_rayHit.GetHit() == false)
        {
            LOG_ERROR("Called RayTriangleHit::GetBarycentricCoordinates() on missed RayTriangleHit. You fucked up.");
            return Float3::zero;
        }
        return m_barycentricCoordinates;
    }
}