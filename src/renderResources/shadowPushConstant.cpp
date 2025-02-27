#include "shadowPushConstant.h"



namespace emberEngine
{
	// Constructor:
	ShadowPushConstant::ShadowPushConstant(uint32_t instanceCount, int shadowMapIndex, const Float4x4& localToClipMatrix)
	{
		this->instanceCount = instanceCount;
		this->localToClipMatrix = localToClipMatrix;
		this->shadowMapIndex = shadowMapIndex;
	}



	// Public methods:
	std::string ShadowPushConstant::ToString()
	{
		std::string output = "ShadowPushConstant:\n";
		output += "Instance Count: " + std::to_string(instanceCount) + "\n";
		output += "Shadow Map Index: " + std::to_string(shadowMapIndex) + "\n";
		output += "Local To Clip Matrix: " + localToClipMatrix.ToString() + "\n";
		return output;
	}
}