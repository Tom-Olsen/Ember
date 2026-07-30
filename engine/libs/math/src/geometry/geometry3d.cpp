#include "geometry3d.h"
#include "logger.h"
#include "mathConstants.h"
#include "mathFunctions.h"
#include "triangle.h"



namespace emberMath
{
	namespace geometry3d
	{
		std::optional<Float3> GetOrhtogonalVector(const Float3& v)
		{
			if (v.IsEpsilonZero())
				return std::nullopt;
			Float3 result = Float3::Cross(v, Float3(1.0f, 0.0f, 0.0f));
			if (math::IsEpsilonZero(result.Length()))
			{
				result = Float3::Cross(v, Float3(0.0f, 1.0f, 0.0f));
				if (math::IsEpsilonZero(result.Length()))
					result = Float3::Cross(v, Float3(0.0f, 0.0f, 1.0f));
			}
			return result;
		}

		std::optional<bool> IsInsidePlane(const Float3& point, const Float3& planeSupport, const Float3& planeNormal)
		{
			if (planeNormal.IsEpsilonZero())
				return std::nullopt;
			float d = Float3::Dot(planeSupport, planeNormal); // plane parameter
            return math::IsEpsilonEqual(Float3::Dot(point, planeNormal), d);
		}

		std::optional<float> PointToPlaneDistance(const Float3& point, const Float3& planeSupport, const Float3& planeNormal)
		{
			if (planeNormal.IsEpsilonZero())
				return std::nullopt;
			Float3 normal = planeNormal.Normalize();
			float d = Float3::Dot(planeSupport, normal); // plane parameter
			return (Float3::Dot(point, normal) - d) / Float3::Dot(normal, normal);
		}

		std::optional<Float3> PointToPlaneProjection(const Float3& point, const Float3& planeSupport, const Float3& planeNormal)
		{
			std::optional<float> distance = PointToPlaneDistance(point, planeSupport, planeNormal);
			if (!distance.has_value())
				return std::nullopt;
			return point - distance.value() * planeNormal.Normalize();
		}

		std::optional<Float3> ReflectPointOnPlane(const Float3& point, const Float3& planeSupport, const Float3& planeNormal)
		{
			std::optional<float> distance = PointToPlaneDistance(point, planeSupport, planeNormal);
			if (!distance.has_value())
				return std::nullopt;
			return point - 2.0f * distance.value() * planeNormal.Normalize();
		}

		std::optional<Float3> LinePlaneIntersection(const Float3& lineSupport, const Float3& lineDirection, const Float3& planeSupport, const Float3& planeNormal)
		{
			if (lineDirection.IsEpsilonZero() || planeNormal.IsEpsilonZero())
				return std::nullopt;
			float denominator = Float3::Dot(lineDirection, planeNormal);
			if (math::IsEpsilonZero(denominator))
				return std::nullopt;
			float d = Float3::Dot(planeSupport, planeNormal);
			float t = (d - Float3::Dot(lineSupport, planeNormal)) / denominator;
			return lineSupport + t * lineDirection;
		}

		RayMeshHit IntersectRayMesh(const Ray& ray, std::span<const Float3> positions, std::span<const Uint3> triangles, const Bounds& bounds)
		{
			if (positions.empty() || triangles.empty() || !bounds.IntersectRay(ray).GetHit())
				return RayMeshHit();

			RayMeshHit closestHit;
			for (std::size_t i = 0; i < triangles.size(); i++)
			{
				const Uint3& indices = triangles[i];
				if (indices.x >= positions.size() || indices.y >= positions.size() || indices.z >= positions.size())
				{
					LOG_ERROR("geometry3d::IntersectRayMesh(): Triangle {} contains an out-of-bounds vertex index.", i);
					continue;
				}

				Triangle triangle(positions[indices.x], positions[indices.y], positions[indices.z]);
				RayTriangleHit triangleHit = triangle.IntersectRay(ray);
				if (!triangleHit.GetHit())
					continue;
				if (closestHit.GetHit() && closestHit.GetDistance() <= triangleHit.GetDistance())
					continue;

				closestHit = RayMeshHit(triangleHit, static_cast<uint32_t>(i));
			}
			return closestHit;
		}
	}
}