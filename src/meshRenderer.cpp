#include "meshRenderer.h"
#include "logger.h"
#include "gameObject.h"



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
	forwardMaterialProperties = std::unique_ptr<MaterialProperties>(forwardMaterial->GetMaterialPropertiesCopy());
}
void MeshRenderer::SetShadowMaterial(Material* shadowMaterial)
{
	this->shadowMaterial = shadowMaterial;
	shadowMaterialProperties = std::unique_ptr<MaterialProperties>(shadowMaterial->GetMaterialPropertiesCopy());
}
void MeshRenderer::SetForwardMatrizes(const Float4x4& cameraViewMatrix, const Float4x4& cameraProjMatrix)
{
	RenderMatrizes renderMatrizes;
	renderMatrizes.modelMatrix = gameObject->transform->GetLocalToWorldMatrix();
	renderMatrizes.viewMatrix = cameraViewMatrix;
	renderMatrizes.projMatrix = cameraProjMatrix;
	renderMatrizes.normalMatrix = gameObject->transform->GetNormalMatrix();
	renderMatrizes.UpdateMvpMatrix();
	forwardMaterialProperties->SetUniform("RenderMatrizes", renderMatrizes);
}
void MeshRenderer::SetShadowMatrizes(const Float4x4& lightViewMatrix, const Float4x4& lightProjMatrix)
{
	RenderMatrizes renderMatrizes;
	renderMatrizes.modelMatrix = gameObject->transform->GetLocalToWorldMatrix();
	renderMatrizes.viewMatrix = lightViewMatrix;
	renderMatrizes.projMatrix = lightProjMatrix;
	renderMatrizes.normalMatrix = gameObject->transform->GetNormalMatrix();
	renderMatrizes.UpdateMvpMatrix();
	shadowMaterialProperties->SetUniform("RenderMatrizes", renderMatrizes);
}



// Getters:
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