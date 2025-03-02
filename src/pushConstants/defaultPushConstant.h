#ifndef __INCLUDE_GUARD_defaultPushConstant_h__
#define __INCLUDE_GUARD_defaultPushConstant_h__
#include "emberMath.h"
#include <string>



namespace emberEngine
{
	/// <summary>
	/// Size limit for push constants is 128 bytes.
	/// Only used for small data that is updated every frame.
	/// </summary>
	struct DefaultPushConstant
	{
		alignas(4) uint32_t instanceCount;
		alignas(4) float time;
		alignas(4) float deltaTime;
		alignas(4) int dLightsCount;
		alignas(4) int sLightsCount;
		alignas(4) int pLightsCount;
		alignas(16) Float4 cameraPosition;

		DefaultPushConstant(uint32_t instanceCount, float time, float deltaTime, int dLightsCount, int sLightsCount, int pLightsCount, const Float3& cameraPosition);
		std::string ToString();
	};
}



#endif // __INCLUDE_GUARD_defaultPushConstant_h__