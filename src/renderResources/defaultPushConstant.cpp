#include "defaultPushConstant.h"
#include "macros.h"



namespace emberEngine
{
	// Constructor:
	DefaultPushConstant::DefaultPushConstant(uint32_t instanceCount, float time, float deltaTime, int dLightsCount, int sLightsCount, int pLightsCount, const Float3& cameraPosition)
	{
		this->instanceCount = instanceCount;
		this->time = time;
		this->deltaTime = deltaTime;
		this->dLightsCount = std::min(dLightsCount, MAX_D_LIGHTS);
		this->sLightsCount = std::min(sLightsCount, MAX_S_LIGHTS);
		this->pLightsCount = std::min(pLightsCount, MAX_P_LIGHTS);
		this->cameraPosition = Float4(cameraPosition, 1.0f);
	}



	// Public methods:
	std::string DefaultPushConstant::ToString()
	{
		std::string output = "DefaultPushConstant:\n";
		output += "Instance Count: " + std::to_string(instanceCount) + "\n";
		output += "Time: " + std::to_string(time) + "\n";
		output += "Delta Time: " + std::to_string(deltaTime) + "\n";
		output += "Directional Lights Count: " + std::to_string(dLightsCount) + "\n";
		output += "Spot Lights Count: " + std::to_string(sLightsCount) + "\n";
		output += "Point Lights Count: " + std::to_string(pLightsCount) + "\n";
		output += "Camera Position: " + cameraPosition.ToString() + "\n";
		return output;
	}
}