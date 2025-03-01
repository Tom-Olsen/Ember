#include "drawCall.h"
#include "camera.h"
#include "directionalLight.h"
#include "pointLight.h"
#include "spotLight.h"



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
	void DrawCall::SetLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights)
	{
		static std::string bufferName = "LightData";
		static std::string arrayName = "directionalLightData";

		for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
			if (directionalLights[i] != nullptr)
			{
				int shadowCascadeCount = (int)directionalLights[i]->GetShadowCascadeCount();
				for (int shadowCascadeIndex = 0; shadowCascadeIndex < shadowCascadeCount; shadowCascadeIndex++)
				{
					Float4x4 worldToClipMatrix = directionalLights[i]->GetProjectionMatrix(shadowCascadeIndex) * directionalLights[i]->GetViewMatrix(shadowCascadeIndex);
					pShaderProperties->SetValue(bufferName, arrayName, i, "worldToClipMatrix", shadowCascadeIndex, worldToClipMatrix);
				}
				pShaderProperties->SetValue(bufferName, arrayName, i, "direction", directionalLights[i]->GetDirection());
				pShaderProperties->SetValue(bufferName, arrayName, i, "softShadows", (int)directionalLights[i]->GetShadowType());
				pShaderProperties->SetValue(bufferName, arrayName, i, "colorIntensity", directionalLights[i]->GetColorIntensity());
				pShaderProperties->SetValue(bufferName, arrayName, i, "shadowCascadeCount", shadowCascadeCount);
			}
		pShaderProperties->SetValue("LightData", "receiveShadows", receiveShadows);
	}
	void DrawCall::SetLightData(const std::array<SpotLight*, MAX_S_LIGHTS>& spotLights)
	{
		static std::string bufferName = "LightData";
		static std::string arrayName = "spotLightData";

		for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
			if (spotLights[i] != nullptr)
			{
				Float4x4 worldToClipMatrix = spotLights[i]->GetProjectionMatrix() * spotLights[i]->GetViewMatrix();
				pShaderProperties->SetValue(bufferName, arrayName, i, "worldToClipMatrix", worldToClipMatrix);
				pShaderProperties->SetValue(bufferName, arrayName, i, "position", spotLights[i]->GetPosition());
				pShaderProperties->SetValue(bufferName, arrayName, i, "softShadows", (int)spotLights[i]->GetShadowType());
				pShaderProperties->SetValue(bufferName, arrayName, i, "colorIntensity", spotLights[i]->GetColorIntensity());
				pShaderProperties->SetValue(bufferName, arrayName, i, "blendStartEnd", spotLights[i]->GetBlendStartEnd());
			}
		pShaderProperties->SetValue(bufferName, "receiveShadows", receiveShadows);
	}
	void DrawCall::SetLightData(const std::array<PointLight*, MAX_P_LIGHTS>& pointLights)
	{
		static std::string bufferName = "LightData";
		static std::string arrayName = "pointLightData";

		for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
			if (pointLights[i] != nullptr)
			{
				for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
				{
					Float4x4 worldToClipMatrix = pointLights[i]->GetProjectionMatrix() * pointLights[i]->GetViewMatrix(faceIndex);
					pShaderProperties->SetValue(bufferName, arrayName, i, "worldToClipMatrix", faceIndex, worldToClipMatrix);
				}
				pShaderProperties->SetValue(bufferName, arrayName, i, "position", pointLights[i]->GetPosition());
				pShaderProperties->SetValue(bufferName, arrayName, i, "softShadows", (int)pointLights[i]->GetShadowType());
				pShaderProperties->SetValue(bufferName, arrayName, i, "colorIntensity", pointLights[i]->GetColorIntensity());
			}
		pShaderProperties->SetValue(bufferName, "receiveShadows", receiveShadows);
	}
}