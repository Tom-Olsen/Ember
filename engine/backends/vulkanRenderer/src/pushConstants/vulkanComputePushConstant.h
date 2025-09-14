#pragma once
#include "emberMath.h"
#include <string>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Size limit for push constants is 128 bytes.
	/// Only used for small data that is updated every frame.
	/// </summary>
	struct ComputePushConstant
	{
		alignas(4) Uint3 threadCount;
		alignas(4) float time;
		alignas(4) float deltaTime;

		ComputePushConstant(Uint3 threadCount, float time, float deltaTime);
		std::string ToString();
	};
}