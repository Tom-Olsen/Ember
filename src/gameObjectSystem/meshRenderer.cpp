#include "meshRenderer.h"
#include "camera.h"
#include "gameObject.h"
#include "materialManager.h"
#include "samplerManager.h"
#include "renderPassManager.h"
#include "logger.h"



// Static members:
Material* MeshRenderer::shadowMaterial = nullptr;
ShadowRenderPass* MeshRenderer::shadowRenderPass = nullptr;



// Constructor:
MeshRenderer::MeshRenderer()
{
	if (shadowMaterial == nullptr)
		shadowMaterial = MaterialManager::GetMaterial("shadowMaterial");
	if (shadowRenderPass == nullptr)
		shadowRenderPass = dynamic_cast<ShadowRenderPass*>(RenderPassManager::GetRenderPass("shadowRenderPass"));

	mesh = nullptr;
	forwardMaterial = nullptr;
	shadowMaterialProperties = std::make_unique<MaterialProperties>(shadowMaterial);
	castShadows = true;
	receiveShadows = true;
}



// Destructor:
MeshRenderer::~MeshRenderer()
{

}



// Public methods:
// Setter:
void MeshRenderer::SetForwardMaterial(Material* forwardMaterial)
{
	this->forwardMaterial = forwardMaterial;
	forwardMaterialProperties = std::make_unique<MaterialProperties>(forwardMaterial);
	
	// Set shadow map sampler and textures:
	forwardMaterialProperties->SetSampler("shadowSampler", SamplerManager::GetSampler("shadowSampler"));
	forwardMaterialProperties->SetTexture2d("shadowMaps", shadowRenderPass->shadowMaps.get());
}
void MeshRenderer::SetForwardRenderMatrizes(Camera* camera)
{
	static std::string name = "RenderMatrizes";

	Float4x4 modelMatrix = gameObject->transform->GetLocalToWorldMatrix();
	Float4x4 cameraViewMatrix = camera->GetViewMatrix();
	Float4x4 cameraProjMatrix = camera->GetProjectionMatrix();
	Float4x4 localToClipMatrix = cameraProjMatrix * cameraViewMatrix * modelMatrix;

	forwardMaterialProperties->SetValue(name, "modelMatrix", modelMatrix);
	forwardMaterialProperties->SetValue(name, "viewMatrix", cameraViewMatrix);
	forwardMaterialProperties->SetValue(name, "projMatrix", cameraProjMatrix);
	forwardMaterialProperties->SetValue(name, "normalMatrix", gameObject->transform->GetNormalMatrix());
	forwardMaterialProperties->SetValue(name, "localToClipMatrix", localToClipMatrix);
}
void MeshRenderer::SetShadowRenderMatrizes(std::array<DirectionalLight*, MAX_D_LIGHTS> directionalLights)
{
	static std::string blockName = "LightMatrizes";
	Float4x4 modelMatrix = gameObject->transform->GetLocalToWorldMatrix();

	for(uint32_t i = 0; i < MAX_D_LIGHTS; i++)
		if (directionalLights[i] != nullptr)
		{
			Float4x4 localToClipMatrix = directionalLights[i]->GetProjectionMatrix() * directionalLights[i]->GetViewMatrix() * modelMatrix;
			shadowMaterialProperties->SetValue(blockName, "localToClipMatrix", i, localToClipMatrix);
		}
}
void MeshRenderer::SetShadowRenderMatrizes(std::array<SpotLight*, MAX_S_LIGHTS> spotLights)
{
	static std::string blockName = "LightMatrizes";
	Float4x4 modelMatrix = gameObject->transform->GetLocalToWorldMatrix();

	for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
		if (spotLights[i] != nullptr)
		{
			Float4x4 localToClipMatrix = spotLights[i]->GetProjectionMatrix() * spotLights[i]->GetViewMatrix() * modelMatrix;
			shadowMaterialProperties->SetValue(blockName, "localToClipMatrix", MAX_D_LIGHTS + i, localToClipMatrix);
		}
}
void MeshRenderer::SetForwardLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights)
{
	static std::string blockName = "LightData";
	static std::string arrayName = "directionalLightData";

	for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
		if (directionalLights[i] != nullptr)
		{
			Float4x4 worldToClipMatrix = directionalLights[i]->GetProjectionMatrix() * directionalLights[i]->GetViewMatrix();
			forwardMaterialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", worldToClipMatrix);
			forwardMaterialProperties->SetValue(blockName, arrayName, i, "direction", directionalLights[i]->GetDirection());
			forwardMaterialProperties->SetValue(blockName, arrayName, i, "colorIntensity", directionalLights[i]->GetColorIntensity());
			forwardMaterialProperties->SetValue(blockName, arrayName, i, "nearClip", directionalLights[i]->GetNearClip());
		}
	forwardMaterialProperties->SetValue(blockName, "receiveShadows", receiveShadows);
}
void MeshRenderer::SetForwardLightData(const std::array<SpotLight*, MAX_S_LIGHTS>& spotLights)
{
	static std::string blockName = "LightData";
	static std::string arrayName = "spotLightData";

	for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
		if (spotLights[i] != nullptr)
		{
			Float4x4 worldToClipMatrix = spotLights[i]->GetProjectionMatrix() * spotLights[i]->GetViewMatrix();
			forwardMaterialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", worldToClipMatrix);
			forwardMaterialProperties->SetValue(blockName, arrayName, i, "position", spotLights[i]->GetPosition());
			forwardMaterialProperties->SetValue(blockName, arrayName, i, "colorIntensity", spotLights[i]->GetColorIntensity());
			forwardMaterialProperties->SetValue(blockName, arrayName, i, "nearClip", spotLights[i]->GetNearClip());
			forwardMaterialProperties->SetValue(blockName, arrayName, i, "blendStartEnd", spotLights[i]->GetBlendStartEnd());
		}
	forwardMaterialProperties->SetValue(blockName, "receiveShadows", receiveShadows);
}



// Forward render pass getters:
Material* MeshRenderer::GetForwardMaterial()
{
	return forwardMaterial;
}
VkDescriptorSet* MeshRenderer::GetForwardDescriptorSets(uint32_t frameIndex)
{
	return &forwardMaterialProperties->descriptorSets[frameIndex];
}
VkPipeline& MeshRenderer::GetForwardPipeline()
{
	return forwardMaterial->pipeline->pipeline;
}
VkPipelineLayout& MeshRenderer::GetForwardPipelineLayout()
{
	return forwardMaterial->pipeline->pipelineLayout;
}

// Shadow render pass getters:
Material* MeshRenderer::GetShadowMaterial()
{
	return shadowMaterial;
}
VkDescriptorSet* MeshRenderer::GetShadowDescriptorSets(uint32_t frameIndex)
{
	return &shadowMaterialProperties->descriptorSets[frameIndex];
}
VkPipeline& MeshRenderer::GetShadowPipeline()
{
	return shadowMaterial->pipeline->pipeline;
}
VkPipelineLayout& MeshRenderer::GetShadowPipelineLayout()
{
	return shadowMaterial->pipeline->pipelineLayout;
}



// Overrides:
std::string MeshRenderer::ToString() const
{
	return "MeshRenderer";
}