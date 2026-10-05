#ifndef __INCLUDE_GUARD_mathRay_hlsli__
#define __INCLUDE_GUARD_mathRay_hlsli__



// Structs:
struct math_Ray
{
	float3 origin;
	float3 direction;
};



// Methods:
float3 mathRay_GetPoint(math_Ray ray, float t)
{
	return ray.origin + t * ray.direction;
}
math_Ray mathRay_Transform(math_Ray ray, float4x4 matrix)
{
	float3 origin = mul(float4(ray.origin, 1.0f), matrix).xyz;
	float3 direction = mul(float4(ray.direction, 0.0f), matrix).xyz;
	math_Ray result = { origin, direction };
	return result;
}
bool mathRay_TryIntersectBounds(math_Ray ray, float3 boundsMin, float3 boundsMax, out float tEnter, out float tExit)
{
	float3 inverseDirection = 1.0f / ray.direction;
	float3 t0 = (boundsMin - ray.origin) * inverseDirection;
	float3 t1 = (boundsMax - ray.origin) * inverseDirection;
	float3 tMin = min(t0, t1);
	float3 tMax = max(t0, t1);

	tEnter = max(tMin.x, max(tMin.y, tMin.z));
	tExit  = min(tMax.x, min(tMax.y, tMax.z));
	return tExit >= max(tEnter, 0.0f);
}
bool mathRay_TryIntersectSphere(math_Ray ray, float3 center, float radius, out float tEnter, out float tExit)
{
	float3 offset = ray.origin - center;
	float a = dot(ray.direction, ray.direction);
	float b = 2.0f * dot(offset, ray.direction);
	float c = dot(offset, offset) - radius * radius;

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
	{
		tEnter = 0.0f;
		tExit = 0.0f;
		return false;
	}
	float sqrtDiscriminant = sqrt(discriminant);
	float inverse2A = 0.5f / a;

	tEnter = (-b - sqrtDiscriminant) * inverse2A;
	tExit = (-b + sqrtDiscriminant) * inverse2A;
	return tExit >= 0.0f;
}
bool mathRay_TryIntersectTriangle(math_Ray ray, float3 vertex0, float3 vertex1, float3 vertex2, out float t, out float2 barycentric)
{
	float3 edge1 = vertex1 - vertex0;
	float3 edge2 = vertex2 - vertex0;
	float3 p = cross(ray.direction, edge2);
	float determinant = dot(edge1, p);

	static const float epsilon = 1.0e-6f;
	if (abs(determinant) < epsilon)
	{
		t = 0.0f;
		barycentric = 0.0f;
		return false;
	}

	float inverseDeterminant = 1.0f / determinant;
	float3 s = ray.origin - vertex0;
	float u = dot(s, p) * inverseDeterminant;
	if (u < 0.0f || u > 1.0f)
	{
		t = 0.0f;
		barycentric = 0.0f;
		return false;
	}

	float3 q = cross(s, edge1);
	float v = dot(ray.direction, q) * inverseDeterminant;
	if (v < 0.0f || u + v > 1.0f)
	{
		t = 0.0f;
		barycentric = 0.0f;
		return false;
	}

	t = dot(edge2, q) * inverseDeterminant;
	barycentric = float2(u, v);
	return t >= 0.0f;
}



#endif // __INCLUDE_GUARD_mathRay_hlsli__