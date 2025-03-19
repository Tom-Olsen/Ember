#include "shadowPushConstant.h"



namespace emberEngine
{
	// Constructor:
	ShadowPushConstant::ShadowPushConstant(uint32_t instanceCount, int shadowMapIndex, const Float4x4& localToWorldMatrix, const Float4x4& worldToClipMatrix)
	{
		this->instanceCount = instanceCount;
		this->shadowMapIndex = shadowMapIndex;
		this->localToWorldMatrix = localToWorldMatrix;
		this->worldToClipMatrix = worldToClipMatrix;
	}



	// Public methods:
	std::string ShadowPushConstant::ToString()
	{
		std::string output = "ShadowPushConstant:\n";
		output += "Instance Count: " + std::to_string(instanceCount) + "\n";
		output += "Shadow Map Index: " + std::to_string(shadowMapIndex) + "\n";
		output += "Local To World Matrix: " + localToWorldMatrix.ToString() + "\n";
		output += "World To Clip Matrix: " + worldToClipMatrix.ToString();
		return output;
	}
}