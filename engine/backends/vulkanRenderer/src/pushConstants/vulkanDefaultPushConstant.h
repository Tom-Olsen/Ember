#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanDefaultPushConstant_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanDefaultPushConstant_h__
#include "iMath.h"
#include <string>



namespace vulkanRendererBackend
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
		alignas(4) int dirLightsCount;
		alignas(4) int posLightsCount;
		alignas(16) iMath::Float4 cameraPosition;

		DefaultPushConstant(uint32_t instanceCount, float time, float deltaTime, int dirLightsCount, int posLightsCount, const iMath::Float3& cameraPosition);
		std::string ToString();
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanDefaultPushConstant_h__