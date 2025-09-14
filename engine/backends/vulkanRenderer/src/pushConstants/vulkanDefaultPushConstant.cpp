#include "vulkanDefaultPushConstant.h"
#include "vulkanContext.h"
#include "vulkanLighting.h"



namespace vulkanRendererBackend
{
	// Constructor:
	DefaultPushConstant::DefaultPushConstant(uint32_t instanceCount, float time, float deltaTime, int dirLightsCount, int posLightsCount, const Float3& cameraPosition)
	{
		this->instanceCount = instanceCount;
		this->time = time;
		this->deltaTime = deltaTime;
		this->dirLightsCount = std::min(dirLightsCount, (int)Lighting::GetMaxDirectionalLights());
		this->posLightsCount = std::min(posLightsCount, (int)Lighting::GetMaxPositionalLights());
		this->cameraPosition = Float4(cameraPosition, 1.0f);
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
		output += "Camera Position: " + cameraPosition.ToString();
		return output;
	}
}