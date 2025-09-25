#include "vulkanShadowPushConstant.h"
#include <sstream>



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

		std::stringstream ss;
		ss << "ShadowPushConstant:\n";
		ss << "Instance Count: " << localToWorldMatrix[{3, 0}] << "\n";
		ss << "Shadow Map Index: " << localToWorldMatrix[{3, 1}] << "\n";
		ss << "Local To World Matrix: " << localToWorld.ToString() << "\n";
		ss << "Compressed Matrix: " << localToWorldMatrix.ToString() << "\n";
		ss << "World To Clip Matrix: " << worldToClipMatrix.ToString();
		return ss.str();
	}
}