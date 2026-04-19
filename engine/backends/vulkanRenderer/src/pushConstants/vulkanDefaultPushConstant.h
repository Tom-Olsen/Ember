#pragma once
#include "emberMath.h"
#include <string>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Size limit for push constants is 128 bytes.
	/// Only used for small data that is updated every frame.
	/// </summary>
		struct DefaultPushConstant
		{
			alignas(4) uint32_t targetIndex;
			alignas(4) uint32_t instanceCount;
			alignas(4) uint32_t receiveShadows;
			alignas(4) float time;
			alignas(4) float deltaTime;

		DefaultPushConstant(uint32_t targetIndex, uint32_t instanceCount, bool receiveShadows, float time, float deltaTime);
		std::string ToString();
	};
}