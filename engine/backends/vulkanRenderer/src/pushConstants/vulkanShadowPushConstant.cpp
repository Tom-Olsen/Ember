#include "vulkanShadowPushConstant.h"



namespace vulkanRendererBackend
{
	// Constructor:
	ShadowPushConstant::ShadowPushConstant(int instanceCount, int shadowMapIndex, const Float4x4& localToWorldMatrix, const Float4x4& worldToClipMatrix)
	{
		this->localToWorldMatrix = localToWorldMatrix;
		this->localToWorldMatrix[{3, 0}] = instanceCount;
		this->localToWorldMatrix[{3, 1}] = shadowMapIndex;
		this->worldToClipMatrix = worldToClipMatrix;
	}



	// Public methods:
	std::string ShadowPushConstant::ToString()
	{
		Float4x4 localToWorld = localToWorldMatrix;
		localToWorld[{3, 0}] = 0;
		localToWorld[{3, 1}] = 0;

		std::string output = "ShadowPushConstant:\n";
		output += "Instance Count: " + std::to_string(localToWorldMatrix[{3, 0}]) + "\n";
		output += "Shadow Map Index: " + std::to_string(localToWorldMatrix[{3, 1}]) + "\n";
		output += "Local To World Matrix: " + localToWorld.ToString() + "\n";
		output += "Compressed Matrix: " + localToWorldMatrix.ToString() + "\n";
		output += "World To Clip Matrix: " + worldToClipMatrix.ToString();
		return output;
	}
}