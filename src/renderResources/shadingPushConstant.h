#ifndef __INCLUDE_GUARD_shadingPushConstant_h__
#define __INCLUDE_GUARD_shadingPushConstant_h__
#include "mathf.h"
#include <string>



/// <summary>
/// Size limit for push constants is 128 bytes.
/// Only used for small data that is updated every frame.
/// </summary>
struct ShadingPushConstant
{
public:
	alignas(4) float time;
	alignas(4) float deltaTime;
	alignas(4) int dLightsCount;
	alignas(4) int sLightsCount;
	alignas(4) int pLightsCount;
	alignas(16) Float4 cameraPosition;

public:
	ShadingPushConstant(float time, float deltaTime, int dLightsCount, int sLightsCount, int pLightsCount, const Float3& cameraPosition);
	std::string ToString();
};



#endif // __INCLUDE_GUARD_shadingPushConstant_h__