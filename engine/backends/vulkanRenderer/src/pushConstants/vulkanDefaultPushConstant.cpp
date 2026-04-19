#include "vulkanDefaultPushConstant.h"
#include "vulkanContext.h"
#include <sstream>



namespace vulkanRendererBackend
{
	// Constructor:
	DefaultPushConstant::DefaultPushConstant(uint32_t targetIndex, uint32_t instanceCount, bool receiveShadows, float time, float deltaTime)
	{
		this->targetIndex = targetIndex;
		this->instanceCount = instanceCount;
		this->receiveShadows = static_cast<uint32_t>(receiveShadows);
		this->time = time;
		this->deltaTime = deltaTime;
	}



	// Public methods:
	std::string DefaultPushConstant::ToString()
	{
		std::stringstream ss;
		ss << "DefaultPushConstant:\n";
		ss << "Target Index: " << targetIndex << "\n";
		ss << "Instance Count: " << instanceCount << "\n";
		ss << "Receive Shadows: " << receiveShadows << "\n";
		ss << "Time: " << time << "\n";
		ss << "Delta Time: " << deltaTime;
		return ss.str();
	}
}