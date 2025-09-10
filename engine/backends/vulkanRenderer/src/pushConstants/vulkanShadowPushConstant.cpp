#include "vulkanShadowPushConstant.h"



namespace vulkanRendererBackend
{
	// Constructor:
	ShadowPushConstant::ShadowPushConstant(int instanceCount, int shadowMapIndex, const iMath::Float4x4& localToWorldMatrix, const iMath::Float4x4& worldToClipMatrix)
	{
		this->localToWorldMatrix = localToWorldMatrix;
		this->localToWorldMatrix[3 + 4 * 0] = instanceCount;	// localToWorldMatrix[{3, 0}] = 0;
		this->localToWorldMatrix[3 + 4 * 1] = shadowMapIndex;	// localToWorldMatrix[{3, 1}] = 0;
		this->worldToClipMatrix = worldToClipMatrix;
	}



	// Public methods:
	std::string ShadowPushConstant::ToString()
	{
		iMath::Float4x4 localToWorld = localToWorldMatrix;
		localToWorld[3 + 4 * 0] = 0;	 // localToWorld[{3, 0}] = 0;
		localToWorld[3 + 4 * 1] = 0;	 // localToWorld[{3, 1}] = 0;

		std::string output = "ShadowPushConstant:\n";
		output += "Instance Count: " + std::to_string(localToWorldMatrix[3 + 4 * 0]) + "\n";
		output += "Shadow Map Index: " + std::to_string(localToWorldMatrix[3 + 4 * 1]) + "\n";
		output += "Local To World Matrix: " + iMath::ToStringFloat4x4(localToWorld) + "\n";
		output += "Compressed Matrix: " + iMath::ToStringFloat4x4(localToWorldMatrix) + "\n";
		output += "World To Clip Matrix: " + iMath::ToStringFloat4x4(worldToClipMatrix);
		return output;
	}
}