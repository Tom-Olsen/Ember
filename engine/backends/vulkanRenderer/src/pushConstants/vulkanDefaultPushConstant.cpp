#include "vulkanDefaultPushConstant.h"
#include "vulkanContext.h"
#include <sstream>



namespace vulkanRendererBackend
{
	// Constructor:
	DefaultPushConstant::DefaultPushConstant(uint32_t instanceCount, float time, float deltaTime, int dirLightsCount, int posLightsCount, const Float3& cameraPosition)
	{
		this->instanceCount = instanceCount;
		this->time = time;
		this->deltaTime = deltaTime;
		this->dirLightsCount = dirLightsCount;
		this->posLightsCount = posLightsCount;
		this->cameraPosition = Float4(cameraPosition, 1.0f);
	}



	// Public methods:
	std::string DefaultPushConstant::ToString()
	{
		std::stringstream ss;
		ss << "DefaultPushConstant:\n";
		ss << "Instance Count: " << instanceCount << "\n";
		ss << "Time: " << time << "\n";
		ss << "Delta Time: " << deltaTime << "\n";
		ss << "Directional Lights Count: " << dirLightsCount << "\n";
		ss << "Positional Lights Count: " << posLightsCount << "\n";
		ss << "Camera Position: " << cameraPosition.ToString();
		return ss.str();
	}
}