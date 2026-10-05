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
	float3 origin = mul(matrix, float4(ray.origin, 1.0f)).xyz;
	float3 direction = mul(matrix, float4(ray.direction, 0.0f)).xyz;
	math_Ray result = { origin, direction };
	return result;
}
bool mathRay_TryIntersectBounds(math_Ray ray, float3 boundsMin, float3 boundsMax, out float tEnter, out float tExit)
{
	tEnter = 0.0f;
	tExit = 0.0f;
	float enterDistance = -asfloat(0x7f800000u); // negative infinity.
	float exitDistance  =  asfloat(0x7f800000u); // positive infinity.

	for (uint axis = 0; axis < 3; axis++)
	{
		// A parallel ray intersects this slab only if its origin is inside it:
		if (ray.direction[axis] == 0.0f)
		{
			if (ray.origin[axis] < boundsMin[axis] || ray.origin[axis] > boundsMax[axis])
				return false;
			continue;
		}

		float t0 = (boundsMin[axis] - ray.origin[axis]) / ray.direction[axis];
		float t1 = (boundsMax[axis] - ray.origin[axis]) / ray.direction[axis];
		enterDistance = max(enterDistance, min(t0, t1));
		exitDistance = min(exitDistance, max(t0, t1));
		if (enterDistance > exitDistance)
			return false;
	}

	if (exitDistance < max(enterDistance, 0.0f))
		return false;
	tEnter = enterDistance;
	tExit = exitDistance;
	return true;
}
bool mathRay_TryIntersectRotatedBounds(math_Ray ray, float4x4 worldToBounds, float3 boundsMin, float3 boundsMax, out float tEnter, out float tExit)
{// worldToBounds maps world space to local bounds space.
	math_Ray boundsRay = mathRay_Transform(ray, worldToBounds);
	return mathRay_TryIntersectBounds(boundsRay, boundsMin, boundsMax, tEnter, tExit);
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