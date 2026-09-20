#include "vulkanComputePushConstant.h"
#include <sstream>



namespace vulkanRendererBackend
{
	// Constructor:
	ComputePushConstant::ComputePushConstant(Uint3 threadCount, float time, float deltaTime, uint32_t sceneColorIndex)
	{
		this->threadCount = threadCount;
		this->time = time;
		this->deltaTime = deltaTime;
		this->sceneColorIndex = sceneColorIndex;
	}

	// Public methods:
	std::string ComputePushConstant::ToString()
	{
		std::stringstream ss;
		ss << "ComputePushConstant:\n";
		ss << "ThreadCount: " << threadCount.ToString() << "\n";
		ss << "Time: " << time << "\n";
		ss << "Delta Time: " << deltaTime << "\n";
		ss << "SceneColorIndex: " << sceneColorIndex;
		return ss.str();
	}
}