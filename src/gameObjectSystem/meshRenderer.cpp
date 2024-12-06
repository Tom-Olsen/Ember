#include "meshRenderer.h"
#include "camera.h"
#include "gameObject.h"
#include "materialManager.h"
#include "samplerManager.h"
#include "textureManager.h"
#include "renderPassManager.h"
#include "logger.h"



// Static members:
Material* MeshRenderer::shadowMaterial = nullptr;
std::unique_ptr<MaterialProperties> MeshRenderer::shadowMaterialProperties = nullptr;



// Constructor:
MeshRenderer::MeshRenderer()
{
	if (shadowMaterial == nullptr)
		shadowMaterial = MaterialManager::GetMaterial("shadow");
	if (shadowMaterialProperties == nullptr)
		shadowMaterialProperties = std::make_unique<MaterialProperties>(shadowMaterial);

	mesh = nullptr;
	material = nullptr;
	castShadows = true;
	receiveShadows = true;
}



// Destructor:
MeshRenderer::~MeshRenderer()
{

}



// Public methods:
// Setter:
void MeshRenderer::SetMaterial(Material* material)
{
	this->material = material;
	materialProperties = std::make_unique<MaterialProperties>(material);
}
void MeshRenderer::SetRenderMatrizes(Camera* camera)
{
	static std::string name = "RenderMatrizes";

	Float4x4 modelMatrix = transform->GetLocalToWorldMatrix();
	Float4x4 cameraViewMatrix = camera->GetViewMatrix();
	Float4x4 cameraProjMatrix = camera->GetProjectionMatrix();
	Float4x4 localToClipMatrix = cameraProjMatrix * cameraViewMatrix * modelMatrix;

	materialProperties->SetValue(name, "modelMatrix", modelMatrix);
	materialProperties->SetValue(name, "viewMatrix", cameraViewMatrix);
	materialProperties->SetValue(name, "projMatrix", cameraProjMatrix);
	materialProperties->SetValue(name, "normalMatrix", transform->GetNormalMatrix());
	materialProperties->SetValue(name, "localToClipMatrix", localToClipMatrix);
}
void MeshRenderer::SetLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights)
{
	static std::string blockName = "LightData";
	static std::string arrayName = "directionalLightData";

	for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
		if (directionalLights[i] != nullptr)
		{
			Float4x4 worldToClipMatrix = directionalLights[i]->GetProjectionMatrix() * directionalLights[i]->GetViewMatrix();
			materialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", worldToClipMatrix);
			materialProperties->SetValue(blockName, arrayName, i, "direction", directionalLights[i]->GetDirection());
			materialProperties->SetValue(blockName, arrayName, i, "colorIntensity", directionalLights[i]->GetColorIntensity());
		}
	materialProperties->SetValue(blockName, "receiveShadows", receiveShadows);
}
void MeshRenderer::SetLightData(const std::array<SpotLight*, MAX_S_LIGHTS>& spotLights)
{
	static std::string blockName = "LightData";
	static std::string arrayName = "spotLightData";

	for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
		if (spotLights[i] != nullptr)
		{
			Float4x4 worldToClipMatrix = spotLights[i]->GetProjectionMatrix() * spotLights[i]->GetViewMatrix();
			materialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", worldToClipMatrix);
			materialProperties->SetValue(blockName, arrayName, i, "position", spotLights[i]->GetPosition());
			materialProperties->SetValue(blockName, arrayName, i, "colorIntensity", spotLights[i]->GetColorIntensity());
			materialProperties->SetValue(blockName, arrayName, i, "blendStartEnd", spotLights[i]->GetBlendStartEnd());
		}
	materialProperties->SetValue(blockName, "receiveShadows", receiveShadows);
}
void MeshRenderer::SetLightData(const std::array<PointLight*, MAX_P_LIGHTS>& pointLights)
{
	static std::string blockName = "LightData";
	static std::string arrayName = "pointLightData";

	for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
		if (pointLights[i] != nullptr)
		{
			for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
			{
				Float4x4 worldToClipMatrix = pointLights[i]->GetProjectionMatrix() * pointLights[i]->GetViewMatrix(faceIndex);
				materialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", faceIndex, worldToClipMatrix);
			}
			materialProperties->SetValue(blockName, arrayName, i, "position", pointLights[i]->GetPosition());
			materialProperties->SetValue(blockName, arrayName, i, "colorIntensity", pointLights[i]->GetColorIntensity());
		}
	materialProperties->SetValue(blockName, "receiveShadows", receiveShadows);
}



// Shading render pass getters:
Material* MeshRenderer::GetMaterial()
{
	return material;
}
VkDescriptorSet* MeshRenderer::GetShadingDescriptorSets(uint32_t frameIndex)
{
	return &materialProperties->descriptorSets[frameIndex];
}
VkPipeline& MeshRenderer::GetShadingPipeline()
{
	return material->pipeline->pipeline;
}
VkPipelineLayout& MeshRenderer::GetShadingPipelineLayout()
{
	return material->pipeline->pipelineLayout;
}

// Shadow render pass getters:
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