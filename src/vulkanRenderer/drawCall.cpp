#include "drawCall.h"
#include "camera.h"



namespace emberEngine
{
	void DrawCall::SetRenderMatrizes(Camera* const pCamera)
	{
		static std::string bufferName = "RenderMatrizes";
		Float4x4 cameraViewMatrix = pCamera->GetViewMatrix();
		Float4x4 cameraProjMatrix = pCamera->GetProjectionMatrix();
		Float4x4 worldToClipMatrix = cameraProjMatrix * cameraViewMatrix;
		Float4x4 localToClipMatrix = worldToClipMatrix * localToWorldMatrix;

		pShaderProperties->SetValue(bufferName, "cb_localToWorldMatrix", localToWorldMatrix);
		pShaderProperties->SetValue(bufferName, "cb_viewMatrix", cameraViewMatrix);
		pShaderProperties->SetValue(bufferName, "cb_projMatrix", cameraProjMatrix);
		pShaderProperties->SetValue(bufferName, "cb_worldToClipMatrix", worldToClipMatrix);
		pShaderProperties->SetValue(bufferName, "cb_localToClipMatrix", localToClipMatrix);
	}
	void DrawCall::SetLightData()
	{
		SetDirectionalLightData();
		SetPositionalLightData();
	}
	void DrawCall::SetDirectionalLightData()
	{
		//std::array<Lighting::DirectionalLight, Lighting::maxDirectionalLights>& directionalLights = Lighting::GetDirectionalLights();
		//static std::string bufferName = "LightData";
		//static std::string arrayName = "directionalLightData";
		//
		//for (uint32_t i = 0; i < Lighting::maxDirectionalLights; i++)
		//{
		//	int shadowCascadeCount = (int)directionalLights[i]->GetShadowCascadeCount();
		//	for (int shadowCascadeIndex = 0; shadowCascadeIndex < shadowCascadeCount; shadowCascadeIndex++)
		//	{
		//		Float4x4 worldToClipMatrix = directionalLights[i]->GetProjectionMatrix(shadowCascadeIndex) * directionalLights[i]->GetViewMatrix(shadowCascadeIndex);
		//		pShaderProperties->SetValue(bufferName, arrayName, i, "worldToClipMatrix", shadowCascadeIndex, worldToClipMatrix);
		//	}
		//	pShaderProperties->SetValue(bufferName, arrayName, i, "direction", directionalLights[i]->GetDirection());
		//	pShaderProperties->SetValue(bufferName, arrayName, i, "softShadows", (int)directionalLights[i]->GetShadowType());
		//	pShaderProperties->SetValue(bufferName, arrayName, i, "colorIntensity", directionalLights[i]->GetColorIntensity());
		//	pShaderProperties->SetValue(bufferName, arrayName, i, "shadowCascadeCount", shadowCascadeCount);
		//}
		//pShaderProperties->SetValue("LightData", "receiveShadows", receiveShadows);
	}
	void DrawCall::SetPositionalLightData()
	{
		std::array<Lighting::PositionalLight, Lighting::maxPositionalLights>& positionalLights = Lighting::GetPositionalLights();
		static std::string bufferName = "LightData";
		static std::string arrayName = "positionalLightData";

		for (uint32_t i = 0; i < Lighting::GetPositionalLightsCount(); i++)
		{
			Float4x4 worldToClipMatrix = positionalLights[i].projectionMatrix * positionalLights[i].viewMatrix;
			pShaderProperties->SetValue(bufferName, arrayName, i, "worldToClipMatrix", worldToClipMatrix);
			pShaderProperties->SetValue(bufferName, arrayName, i, "position", positionalLights[i].position);
			pShaderProperties->SetValue(bufferName, arrayName, i, "softShadows", (int)positionalLights[i].shadowType);
			pShaderProperties->SetValue(bufferName, arrayName, i, "colorIntensity", Float4(positionalLights[i].color, positionalLights[i].intensity));
			pShaderProperties->SetValue(bufferName, arrayName, i, "blendStartEnd", Float2(positionalLights[i].blendStart, positionalLights[i].blendEnd));
		}
		pShaderProperties->SetValue(bufferName, "receiveShadows", receiveShadows);
	}
}