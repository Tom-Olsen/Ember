#pragma once
#ifndef __INCLUDE_GUARD_mathf_h__
#define __INCLUDE_GUARD_mathf_h__
#include <stdint.h>



// Constants:
constexpr float PI = 3.14159265358979323846f;



// Enums:
enum CoordinateSystem
{
	local,
	world
};



// Multi indexing:
struct Index2
{
	uint32_t i, j;
};



// Class/Struct independent functions:
namespace mathf
{
	float Clamp(float value, float min, float max);
	float ToDegrees(float radians);
	float ToRadians(float degrees);
	float Sign(float value);
	float Sin(float radiant);
	float Cos(float radiant);
	float Tan(float radiant);
	float Asin(float value);
	float Acos(float value);
	float Atan(float z);
	float Atan2(float y, float x);
	template<int N>
	float IntegerPow(float a);
	template<int N>
	float Factorial();
}



// Include all mathf headers:
#include "int2.h"
#include "int3.h"
#include "uint3.h"
#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "float3x3.h"
#include "float4x4.h"
#endif // __INCLUDE_GUARD_mathf_h__