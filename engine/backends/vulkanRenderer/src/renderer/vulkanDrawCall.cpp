#include "vulkanDrawCall.h"
#include "vulkanDescriptorSetBinding.h"
#include <array>



namespace vulkanRendererBackend
{
    // Public methods:
	void DrawCall::SetRenderMatrizes(const Float4x4& viewMatrix, const Float4x4& projectionMatrix)
	{
		static std::string bufferName = "RenderMatrizes";
		Float4x4 worldToClipMatrix = projectionMatrix * viewMatrix;
		Float4x4 localToClipMatrix = worldToClipMatrix * localToWorldMatrix;

		pDescriptorSetBinding->SetFloat4x4(bufferName, "cb_localToWorldMatrix", localToWorldMatrix);
		pDescriptorSetBinding->SetFloat4x4(bufferName, "cb_viewMatrix", viewMatrix);
		pDescriptorSetBinding->SetFloat4x4(bufferName, "cb_projMatrix", projectionMatrix);
		pDescriptorSetBinding->SetFloat4x4(bufferName, "cb_worldToClipMatrix", worldToClipMatrix);
		pDescriptorSetBinding->SetFloat4x4(bufferName, "cb_localToClipMatrix", localToClipMatrix);
	}
	void DrawCall::SetLightData(std::vector<emberCommon::DirectionalLight>& directionalLights, std::vector<emberCommon::PositionalLight>& positionalLights)
	{
		SetDirectionalLightData(directionalLights);
		SetPositionalLightData(positionalLights);
		pDescriptorSetBinding->SetBool("LightData", "receiveShadows", receiveShadows);
	}



    // Private methods:
	void DrawCall::SetDirectionalLightData(std::vector<emberCommon::DirectionalLight>& directionalLights)
	{
		static std::string bufferName = "LightData";
		static std::string arrayName = "directionalLightData";
		for (uint32_t i = 0; i < directionalLights.size(); i++)
		{
			pDescriptorSetBinding->SetFloat4x4(bufferName, arrayName, i, "worldToClipMatrix", directionalLights[i].worldToClipMatrix);
			pDescriptorSetBinding->SetFloat3(bufferName, arrayName, i, "direction", directionalLights[i].direction);
			pDescriptorSetBinding->SetInt(bufferName, arrayName, i, "shadowType", (int)directionalLights[i].shadowType);
			pDescriptorSetBinding->SetFloat4(bufferName, arrayName, i, "colorIntensity", Float4(directionalLights[i].color, directionalLights[i].intensity));
		}
	}
	void DrawCall::SetPositionalLightData(std::vector<emberCommon::PositionalLight>& positionalLights)
	{
		static std::string bufferName = "LightData";
		static std::string arrayName = "positionalLightData";
		for (uint32_t i = 0; i < positionalLights.size(); i++)
		{
			pDescriptorSetBinding->SetFloat4x4(bufferName, arrayName, i, "worldToClipMatrix", positionalLights[i].worldToClipMatrix);
			pDescriptorSetBinding->SetFloat3(bufferName, arrayName, i, "position", positionalLights[i].position);
			pDescriptorSetBinding->SetInt(bufferName, arrayName, i, "shadowType", (int)positionalLights[i].shadowType);
			pDescriptorSetBinding->SetFloat4(bufferName, arrayName, i, "colorIntensity", Float4(positionalLights[i].color, positionalLights[i].intensity));
			pDescriptorSetBinding->SetFloat2(bufferName, arrayName, i, "blendStartEnd", Float2(positionalLights[i].blendStart, positionalLights[i].blendEnd));
		}
	}
}