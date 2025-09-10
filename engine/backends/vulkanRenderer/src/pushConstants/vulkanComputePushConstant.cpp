#include "vulkanComputePushConstant.h"



namespace vulkanRendererBackend
{
	// Constructor:
	ComputePushConstant::ComputePushConstant(iMath::Uint3 threadCount, float time, float deltaTime)
	{
		this->threadCount = threadCount;
		this->time = time;
		this->deltaTime = deltaTime;
	}

	// Public methods:
	std::string ComputePushConstant::ToString()
	{
		std::string output = "ComputePushConstant:\n";
		output += "ThreadCount: " + iMath::ToStringUint3(threadCount) + "\n";
		output += "Time: " + std::to_string(time) + "\n";
		output += "Delta Time: " + std::to_string(deltaTime);
		return output;
	}
}