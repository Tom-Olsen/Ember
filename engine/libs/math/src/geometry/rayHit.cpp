#include "rayHit.h"
#include "logger.h"
#include "mathConstants.h"



namespace emberMath
{
    // Public methods:
    // Constructors:
    RayHit::RayHit()
        : m_hit(false), m_distance(math::maxValue), m_point(Float3::zero), m_normal(Float3::zero) {}
    RayHit::RayHit(float distance, const Float3& point, const Float3& normal)
        : m_hit(true), m_distance(distance), m_point(point), m_normal(normal) {}



    // Getters:
    bool RayHit::GetHit()
    {
        return m_hit;
    }
    float RayHit::GetDistance()
    {
        if (m_hit == false)
        {
            LOG_ERROR("Called RayHit::GetDistance() on missed RayHit. You fucked up.");
            return math::maxValue;
        }
        return m_distance;
    }
    Float3 RayHit::GetPoint()
    {
        if (m_hit == false)
        {
            LOG_ERROR("Called RayHit::GetPoint() on missed RayHit. You fucked up.");
            return Float3::zero;
        }
        return m_point;
    }
    Float3 RayHit::GetNormal()
    {
        if (m_hit == false)
        {
            LOG_ERROR("Called RayHit::GetNormal() on missed RayHit. You fucked up.");
            return Float3::zero;
        }
        return m_normal;
    }
}