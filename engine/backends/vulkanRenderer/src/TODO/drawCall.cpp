#include "drawCall.h"
#include "lighting.h"
#include "shaderProperties.h"



namespace emberEngine
{
	void DrawCall::SetRenderMatrizes(const Float4x4& viewMatrix, const Float4x4& projectionMatrix)
	{
		static std::string bufferName = "RenderMatrizes";
		Float4x4 worldToClipMatrix = projectionMatrix * viewMatrix;
		Float4x4 localToClipMatrix = worldToClipMatrix * localToWorldMatrix;

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
	void DrawCall::SetDirectionalLightData()
	{
		std::array<Lighting::DirectionalLight, Lighting::maxDirectionalLights>& directionalLights = Lighting::GetDirectionalLights();
		static std::string bufferName = "LightData";
		static std::string arrayName = "directionalLightData";
		
		for (uint32_t i = 0; i < Lighting::GetDirectionalLightsCount(); i++)
		{
			pShaderProperties->SetValue(bufferName, arrayName, i, "worldToClipMatrix", directionalLights[i].worldToClipMatrix);
			pShaderProperties->SetValue(bufferName, arrayName, i, "direction", directionalLights[i].direction);
			pShaderProperties->SetValue(bufferName, arrayName, i, "shadowType", (int)directionalLights[i].shadowType);
			pShaderProperties->SetValue(bufferName, arrayName, i, "colorIntensity", Float4(directionalLights[i].color, directionalLights[i].intensity));
		}
	}
	void DrawCall::SetPositionalLightData()
	{
		std::array<Lighting::PositionalLight, Lighting::maxPositionalLights>& positionalLights = Lighting::GetPositionalLights();
		static std::string bufferName = "LightData";
		static std::string arrayName = "positionalLightData";

		for (uint32_t i = 0; i < Lighting::GetPositionalLightsCount(); i++)
		{
			pShaderProperties->SetValue(bufferName, arrayName, i, "worldToClipMatrix", positionalLights[i].worldToClipMatrix);
			pShaderProperties->SetValue(bufferName, arrayName, i, "position", positionalLights[i].position);
			pShaderProperties->SetValue(bufferName, arrayName, i, "shadowType", (int)positionalLights[i].shadowType);
			pShaderProperties->SetValue(bufferName, arrayName, i, "colorIntensity", Float4(positionalLights[i].color, positionalLights[i].intensity));
			pShaderProperties->SetValue(bufferName, arrayName, i, "blendStartEnd", Float2(positionalLights[i].blendStart, positionalLights[i].blendEnd));
		}
	}
}