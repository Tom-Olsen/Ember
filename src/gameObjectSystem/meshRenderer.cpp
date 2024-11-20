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
#include <iostream>
void MeshRenderer::SetForwardMaterial(Material* forwardMaterial)
{
	this->forwardMaterial = forwardMaterial;
	forwardMaterialProperties = std::make_unique<MaterialProperties>(forwardMaterial);
	
	// Set shadow map sampler and textures:
	forwardMaterialProperties->SetSamplerForAllFrames("shadowSampler", SamplerManager::GetSampler("shadowSampler"));
	forwardMaterialProperties->SetTexture2dForAllFrames("shadowMaps", shadowRenderPass->shadowMaps.get());
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
	static std::string name = "LightMatrizes";
	Float4x4 modelMatrix = gameObject->transform->GetLocalToWorldMatrix();

	for(uint32_t i = 0; i < MAX_D_LIGHTS; i++)
		if (directionalLights[i] != nullptr)
		{
			Float4x4 localToClipMatrix = directionalLights[i]->GetProjectionMatrix() * directionalLights[i]->GetViewMatrix() * modelMatrix;
			shadowMaterialProperties->SetValue(name, "localToClipMatrixTest", i, localToClipMatrix);
		}
}
void MeshRenderer::SetForwardLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights)
{
	static std::string name = "LightData";

	for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
		if (directionalLights[i] != nullptr)
		{
			Float4x4 worldToClipMatrix = directionalLights[i]->GetProjectionMatrix() * directionalLights[i]->GetViewMatrix();
			Float4 directionIntensity = Float4(directionalLights[i]->gameObject->transform->GetForward(), directionalLights[i]->GetIntensity());

			forwardMaterialProperties->SetValue(name, "worldToClipMatrix", i, worldToClipMatrix);
			forwardMaterialProperties->SetValue(name, "directionIntensity", i, directionIntensity);
			forwardMaterialProperties->SetValue(name, "lightColor", i, directionalLights[i]->GetColor());
		}
	forwardMaterialProperties->SetValue(name, "receiveShadows", receiveShadows);
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