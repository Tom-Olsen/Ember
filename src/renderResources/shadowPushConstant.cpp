#include "shadowPushConstant.h"



namespace emberEngine
{
	// Constructor:
	ShadowPushConstant::ShadowPushConstant(int shadowMapIndex, const Float4x4& localToClipMatrix)
	{
		this->localToClipMatrix = localToClipMatrix;
		this->shadowMapIndex = shadowMapIndex;
	}



	// Public methods:
	std::string ShadowPushConstant::ToString()
	{
		std::string output = "ShadowPushConstant:\n";
		output += "ShadowMapIndex: " + std::to_string(shadowMapIndex) + "\n";
		output += "LocalToClipMatrix: " + localToClipMatrix.ToString() + "\n";
		return output;
	}
}