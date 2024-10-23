#include "meshRenderer.h"
#include "logger.h"
#include "gameObject.h"



// Constructor:
MeshRenderer::MeshRenderer()
{
	mesh = nullptr;
	material = nullptr;
}



// Destructor:
MeshRenderer::~MeshRenderer()
{

}



// Public methods:
void MeshRenderer::SetMaterial(Material* material)
{
	this->material = material;
	materialProperties = std::unique_ptr<MaterialProperties>(material->GetMaterialPropertiesCopy());
}
VkDescriptorSet* MeshRenderer::GetDescriptorSets(uint32_t frameIndex)
{
	return &materialProperties->descriptorSets[frameIndex];
}
VkPipeline& MeshRenderer::GetPipeline()
{
	return material->pipeline->pipeline;
}
VkPipelineLayout& MeshRenderer::GetPipelineLayout()
{
	return material->pipeline->pipelineLayout;
}



// Overrides:
void MeshRenderer::Update()
{
	if (camera != nullptr)
	{
		RenderMatrizes renderMatrizes;
		renderMatrizes.modelMatrix = gameObject->transform->GetLocalToWorldMatrix();
		renderMatrizes.viewMatrix = camera->GetViewMatrix();
		renderMatrizes.projMatrix = camera->GetProjectionMatrix();
		renderMatrizes.normalMatrix = gameObject->transform->GetLocalToWorldMatrix();
		renderMatrizes.UpdateMvpMatrix();
		materialProperties->SetUniform("RenderMatrizes", renderMatrizes);
	}
}
std::string MeshRenderer::ToString() const
{
	return "MeshRenderer";
}