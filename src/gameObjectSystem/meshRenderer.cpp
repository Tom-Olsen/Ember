#include "meshRenderer.h"
#include "camera.h"
#include "gameObject.h"
#include "logger.h"



// Constructor:
MeshRenderer::MeshRenderer()
{
	mesh = nullptr;
	forwardMaterial = nullptr;
	shadowMaterial = nullptr;
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
}
void MeshRenderer::SetShadowMaterial(Material* shadowMaterial)
{
	this->shadowMaterial = shadowMaterial;
	shadowMaterialProperties = std::make_unique<MaterialProperties>(shadowMaterial);
}
void MeshRenderer::SetForwardRenderMatrizes(const Float4x4& cameraViewMatrix, const Float4x4& cameraProjMatrix)
{
	static std::string name = "RenderMatrizes";

	Float4x4 modelMatrix = gameObject->transform->GetLocalToWorldMatrix();
	Float4x4 localToClipMatrix = cameraProjMatrix * cameraViewMatrix * modelMatrix;

	forwardMaterialProperties->SetValue(name, "modelMatrix", modelMatrix);
	forwardMaterialProperties->SetValue(name, "viewMatrix", cameraViewMatrix);
	forwardMaterialProperties->SetValue(name, "projMatrix", cameraProjMatrix);
	forwardMaterialProperties->SetValue(name, "normalMatrix", gameObject->transform->GetNormalMatrix());
	forwardMaterialProperties->SetValue(name, "localToClipMatrix", localToClipMatrix);
}
void MeshRenderer::SetShadowRenderMatrizes(const Float4x4& lightViewMatrix, const Float4x4& lightProjMatrix)
{
	static std::string name = "DirectionalLightRenderMatrizes";

	Float4x4 modelMatrix = gameObject->transform->GetLocalToWorldMatrix();
	Float4x4 localToClipMatrix = lightProjMatrix * lightViewMatrix * modelMatrix;

	shadowMaterialProperties->SetValue(name, "modelMatrix", modelMatrix);
	shadowMaterialProperties->SetValue(name, "viewMatrix", lightViewMatrix);
	shadowMaterialProperties->SetValue(name, "projMatrix", lightProjMatrix);
	shadowMaterialProperties->SetValue(name, "localToClipMatrix", localToClipMatrix);
}
void MeshRenderer::SetForwardLightData(const std::array<DirectionalLight*, 2>& directionalLights)
{
	static std::string name = "LightData";

	Float4x4 viewMatrix = directionalLights[0]->GetViewMatrix();
	Float4x4 projMatrix = directionalLights[0]->GetProjectionMatrix();
	Float4x4 worldToClipMatrix = projMatrix * viewMatrix;
	Float4 directionIntensity = Float4(directionalLights[0]->gameObject->transform->GetForward(), directionalLights[0]->GetIntensity());

	forwardMaterialProperties->SetValue(name, "viewMatrix", viewMatrix);
	forwardMaterialProperties->SetValue(name, "projMatrix", projMatrix);
	forwardMaterialProperties->SetValue(name, "worldToClipMatrix", worldToClipMatrix);
	forwardMaterialProperties->SetValue(name, "directionIntensity", directionIntensity);
	forwardMaterialProperties->SetValue(name, "lightColor", directionalLights[0]->GetColor());
}


// Getters:
Material* MeshRenderer::GetForwardMaterial()
{
	return forwardMaterial;
}
Material* MeshRenderer::GetShadowMaterial()
{
	return shadowMaterial;
}
VkDescriptorSet* MeshRenderer::GetForwardDescriptorSets(uint32_t frameIndex)
{
	return &forwardMaterialProperties->descriptorSets[frameIndex];
}
VkDescriptorSet* MeshRenderer::GetShadowDescriptorSets(uint32_t frameIndex)
{
	return &shadowMaterialProperties->descriptorSets[frameIndex];
}
VkPipeline& MeshRenderer::GetForwardPipeline()
{
	return forwardMaterial->pipeline->pipeline;
}
VkPipeline& MeshRenderer::GetShadowPipeline()
{
	return shadowMaterial->pipeline->pipeline;
}
VkPipelineLayout& MeshRenderer::GetForwardPipelineLayout()
{
	return forwardMaterial->pipeline->pipelineLayout;
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