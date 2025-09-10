#include "vulkanDefaultPushConstant.h"
#include "vulkanContext.h"



namespace vulkanRendererBackend
{
	// Constructor:
	DefaultPushConstant::DefaultPushConstant(uint32_t instanceCount, float time, float deltaTime, int dirLightsCount, int posLightsCount, const iMath::Float3& cameraPosition)
	{
		this->instanceCount = instanceCount;
		this->time = time;
		this->deltaTime = deltaTime;
		this->dirLightsCount = std::min(dirLightsCount, (int)Context::maxDirectionalLights);
		this->posLightsCount = std::min(posLightsCount, (int)Context::maxPositionalLights);
		this->cameraPosition = iMath::Float4{ cameraPosition[0], cameraPosition[1], cameraPosition[2], 1.0f };
	}



	// Public methods:
	std::string DefaultPushConstant::ToString()
	{
		std::string output = "DefaultPushConstant:\n";
		output += "Instance Count: " + std::to_string(instanceCount) + "\n";
		output += "Time: " + std::to_string(time) + "\n";
		output += "Delta Time: " + std::to_string(deltaTime) + "\n";
		output += "Directional Lights Count: " + std::to_string(dirLightsCount) + "\n";
		output += "Positional Lights Count: " + std::to_string(posLightsCount) + "\n";
		output += "Camera Position: " + iMath::ToStringFloat4(cameraPosition);
		return output;
	}
}