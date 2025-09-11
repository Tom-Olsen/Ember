#include "drawCall.h"
#include "lighting.h"
#include "shaderProperties.h"
#include <array>



namespace vulkanRendererBackend
{
    // Public methods:
	void DrawCall::SetRenderMatrizes(const iMath::Float4x4& viewMatrix, const iMath::Float4x4& projectionMatrix)
	{
		static std::string bufferName = "RenderMatrizes";
		iMath::Float4x4 worldToClipMatrix = iMath::Multiply(projectionMatrix, viewMatrix);
		iMath::Float4x4 localToClipMatrix = iMath::Multiply(worldToClipMatrix, localToWorldMatrix);

		pShaderProperties->SetValue(bufferName, "cb_localToWorldMatrix", localToWorldMatrix);
		pShaderProperties->SetValue(bufferName, "cb_viewMatrix", viewMatrix);
		pShaderProperties->SetValue(bufferName, "cb_projMatrix", projectionMatrix);
		pShaderProperties->SetValue(bufferName, "cb_worldToClipMatrix", worldToClipMatrix);
		pShaderProperties->SetValue(bufferName, "cb_localToClipMatrix", localToClipMatrix);
	}
	void DrawCall::SetLightData()
	{
		SetDirectionalLightData();
		SetPositionalLightData();
		pShaderProperties->SetValue("LightData", "receiveShadows", receiveShadows);
	}



    // Private methods:
	void DrawCall::SetDirectionalLightData()
	{
		std::vector<Lighting::DirectionalLight>& directionalLights = Lighting::GetDirectionalLights();
		static std::string bufferName = "LightData";
		static std::string arrayName = "directionalLightData";
		
		for (uint32_t i = 0; i < Lighting::GetDirectionalLightsCount(); i++)
		{
			pShaderProperties->SetValue(bufferName, arrayName, i, "worldToClipMatrix", directionalLights[i].worldToClipMatrix);
			pShaderProperties->SetValue(bufferName, arrayName, i, "direction", directionalLights[i].direction);
			pShaderProperties->SetValue(bufferName, arrayName, i, "shadowType", (int)directionalLights[i].shadowType);
			pShaderProperties->SetValue(bufferName, arrayName, i, "colorIntensity", iMath::Float4{ directionalLights[i].color[0], directionalLights[i].color[1], directionalLights[i].color[2], directionalLights[i].intensity });
		}
	}
	void DrawCall::SetPositionalLightData()
	{
		std::vector<Lighting::PositionalLight>& positionalLights = Lighting::GetPositionalLights();
		static std::string bufferName = "LightData";
		static std::string arrayName = "positionalLightData";

		for (uint32_t i = 0; i < Lighting::GetPositionalLightsCount(); i++)
		{
			pShaderProperties->SetValue(bufferName, arrayName, i, "worldToClipMatrix", positionalLights[i].worldToClipMatrix);
			pShaderProperties->SetValue(bufferName, arrayName, i, "position", positionalLights[i].position);
			pShaderProperties->SetValue(bufferName, arrayName, i, "shadowType", (int)positionalLights[i].shadowType);
			pShaderProperties->SetValue(bufferName, arrayName, i, "colorIntensity", iMath::Float4{ positionalLights[i].color[0],positionalLights[i].color[1],positionalLights[i].color[2], positionalLights[i].intensity});
			pShaderProperties->SetValue(bufferName, arrayName, i, "blendStartEnd", iMath::Float2{ positionalLights[i].blendStart, positionalLights[i].blendEnd });
		}
	}
}