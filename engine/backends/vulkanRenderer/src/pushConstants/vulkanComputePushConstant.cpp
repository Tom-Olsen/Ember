#include "vulkanComputePushConstant.h"
#include <sstream>



namespace vulkanRendererBackend
{
	// Constructor:
	ComputePushConstant::ComputePushConstant(Uint3 threadCount, float time, float deltaTime)
	{
		this->threadCount = threadCount;
		this->time = time;
		this->deltaTime = deltaTime;
	}

	// Public methods:
	std::string ComputePushConstant::ToString()
	{
		std::stringstream ss;
		ss << "ComputePushConstant:\n";
		ss << "ThreadCount: " << threadCount.ToString() << "\n";
		ss << "Time: " << time << "\n";
		ss << "Delta Time: " << deltaTime;
		return ss.str();
	}
}