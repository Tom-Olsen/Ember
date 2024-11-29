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
	shadingMaterial = nullptr;
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
void MeshRenderer::SetShadingMaterial(Material* shadingMaterial)
{
	this->shadingMaterial = shadingMaterial;
	shadingMaterialProperties = std::make_unique<MaterialProperties>(shadingMaterial);

	// Set shadow map sampler and textures:
	shadingMaterialProperties->SetSampler("shadowSampler", SamplerManager::GetSampler("shadowSampler"));
	shadingMaterialProperties->SetTexture2d("shadowMaps", shadowRenderPass->shadowMaps.get());
}
void MeshRenderer::SetShadingRenderMatrizes(Camera* camera)
{
	static std::string name = "RenderMatrizes";

	Float4x4 modelMatrix = gameObject->transform->GetLocalToWorldMatrix();
	Float4x4 cameraViewMatrix = camera->GetViewMatrix();
	Float4x4 cameraProjMatrix = camera->GetProjectionMatrix();
	Float4x4 localToClipMatrix = cameraProjMatrix * cameraViewMatrix * modelMatrix;

	shadingMaterialProperties->SetValue(name, "modelMatrix", modelMatrix);
	shadingMaterialProperties->SetValue(name, "viewMatrix", cameraViewMatrix);
	shadingMaterialProperties->SetValue(name, "projMatrix", cameraProjMatrix);
	shadingMaterialProperties->SetValue(name, "normalMatrix", gameObject->transform->GetNormalMatrix());
	shadingMaterialProperties->SetValue(name, "localToClipMatrix", localToClipMatrix);
}
void MeshRenderer::SetShadowRenderMatrizes(std::array<DirectionalLight*, MAX_D_LIGHTS> directionalLights)
{
	static std::string blockName = "LightMatrizes";
	Float4x4 modelMatrix = gameObject->transform->GetLocalToWorldMatrix();

	for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
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
void MeshRenderer::SetShadowRenderMatrizes(std::array<PointLight*, MAX_P_LIGHTS> pointLights)
{
	static std::string blockName = "LightMatrizes";
	Float4x4 modelMatrix = gameObject->transform->GetLocalToWorldMatrix();

	for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
		if (pointLights[i] != nullptr)
		{
			for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
			{
				// Apply rotation of camera model in local sapce, to cover the 6 sides of a cube:
				Float4x4 localToClipMatrix = pointLights[i]->GetProjectionMatrix() * pointLights[i]->GetViewMatrix(faceIndex) * modelMatrix;
				shadowMaterialProperties->SetValue(blockName, "localToClipMatrix", MAX_D_LIGHTS + MAX_S_LIGHTS + 6 * i + faceIndex, localToClipMatrix);
			}
		}
}
void MeshRenderer::SetShadingLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights)
{
	static std::string blockName = "LightData";
	static std::string arrayName = "directionalLightData";

	for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
		if (directionalLights[i] != nullptr)
		{
			Float4x4 worldToClipMatrix = directionalLights[i]->GetProjectionMatrix() * directionalLights[i]->GetViewMatrix();
			shadingMaterialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", worldToClipMatrix);
			shadingMaterialProperties->SetValue(blockName, arrayName, i, "direction", directionalLights[i]->GetDirection());
			shadingMaterialProperties->SetValue(blockName, arrayName, i, "colorIntensity", directionalLights[i]->GetColorIntensity());
		}
	shadingMaterialProperties->SetValue(blockName, "receiveShadows", receiveShadows);
}
void MeshRenderer::SetShadingLightData(const std::array<SpotLight*, MAX_S_LIGHTS>& spotLights)
{
	static std::string blockName = "LightData";
	static std::string arrayName = "spotLightData";

	for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
		if (spotLights[i] != nullptr)
		{
			Float4x4 worldToClipMatrix = spotLights[i]->GetProjectionMatrix() * spotLights[i]->GetViewMatrix();
			shadingMaterialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", worldToClipMatrix);
			shadingMaterialProperties->SetValue(blockName, arrayName, i, "position", spotLights[i]->GetPosition());
			shadingMaterialProperties->SetValue(blockName, arrayName, i, "colorIntensity", spotLights[i]->GetColorIntensity());
			shadingMaterialProperties->SetValue(blockName, arrayName, i, "blendStartEnd", spotLights[i]->GetBlendStartEnd());
		}
	shadingMaterialProperties->SetValue(blockName, "receiveShadows", receiveShadows);
}
void MeshRenderer::SetShadingLightData(const std::array<PointLight*, MAX_P_LIGHTS>& pointLights)
{
	static std::string blockName = "LightData";
	static std::string arrayName = "pointLightData";

	for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
		if (pointLights[i] != nullptr)
		{
			for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
			{
				Float4x4 worldToClipMatrix = pointLights[i]->GetProjectionMatrix() * pointLights[i]->GetViewMatrix(faceIndex);
				shadingMaterialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", faceIndex, worldToClipMatrix);
			}
			shadingMaterialProperties->SetValue(blockName, arrayName, i, "position", pointLights[i]->GetPosition());
			shadingMaterialProperties->SetValue(blockName, arrayName, i, "colorIntensity", pointLights[i]->GetColorIntensity());
		}
	shadingMaterialProperties->SetValue(blockName, "receiveShadows", receiveShadows);
}



// Shading render pass getters:
Material* MeshRenderer::GetShadingMaterial()
{
	return shadingMaterial;
}
VkDescriptorSet* MeshRenderer::GetShadingDescriptorSets(uint32_t frameIndex)
{
	return &shadingMaterialProperties->descriptorSets[frameIndex];
}
VkPipeline& MeshRenderer::GetShadingPipeline()
{
	return shadingMaterial->pipeline->pipeline;
}
VkPipelineLayout& MeshRenderer::GetShadingPipelineLayout()
{
	return shadingMaterial->pipeline->pipelineLayout;
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