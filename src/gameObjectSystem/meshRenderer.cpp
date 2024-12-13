#include "meshRenderer.h"
#include "camera.h"
#include "directionalLight.h"
#include "material.h"
#include "materialManager.h"
#include "materialProperties.h"
#include "pipeline.h"
#include "pointLight.h"
#include "samplerManager.h"
#include "spotLight.h"
#include "textureManager.h"
#include "renderPassManager.h"



// Static members:
Material* MeshRenderer::m_pShadowMaterial = nullptr;
std::unique_ptr<MaterialProperties> MeshRenderer::m_pShadowMaterialProperties = nullptr;



// Constructor/Destructor:
MeshRenderer::MeshRenderer()
{
	m_castShadows = true;
	m_receiveShadows = true;
	m_hasErrorMaterial = true;

	m_pMesh = nullptr;
	m_pMaterial = MaterialManager::GetMaterial("error");

	if (m_pShadowMaterial == nullptr)
		m_pShadowMaterial = MaterialManager::GetMaterial("shadow");
	if (m_pShadowMaterialProperties == nullptr)
		m_pShadowMaterialProperties = std::make_unique<MaterialProperties>(m_pShadowMaterial);
}
MeshRenderer::~MeshRenderer()
{

}



// Public methods:
// Setter:
void MeshRenderer::SetCastShadows(bool castShadows)
{
	m_castShadows = castShadows;
}
void MeshRenderer::SetReceiveShadows(bool receiveShadows)
{
	m_receiveShadows = receiveShadows;
}
void MeshRenderer::SetMesh(Mesh* pMesh)
{
	m_pMesh = pMesh;
}
void MeshRenderer::SetMaterial(Material* pMaterial)
{
	if (pMaterial == nullptr)
	{
		m_hasErrorMaterial = true;
		if (m_pMaterial != MaterialManager::GetMaterial("error"))
		{
			m_pMaterial = MaterialManager::GetMaterial("error");
			m_pMaterialProperties = std::make_unique<MaterialProperties>(m_pMaterial);
		}
	}
	else if(m_pMaterial != pMaterial)
	{
		m_hasErrorMaterial = false;
		m_pMaterial = pMaterial;
		m_pMaterialProperties = std::make_unique<MaterialProperties>(m_pMaterial);
	}
}
void MeshRenderer::SetRenderMatrizes(Camera* const pCamera)
{
	static std::string name = "RenderMatrizes";

	Float4x4 modelMatrix = GetTransform()->GetLocalToWorldMatrix();
	Float4x4 cameraViewMatrix = pCamera->GetViewMatrix();
	Float4x4 cameraProjMatrix = pCamera->GetProjectionMatrix();
	Float4x4 localToClipMatrix = cameraProjMatrix * cameraViewMatrix * modelMatrix;

	m_pMaterialProperties->SetValue(name, "modelMatrix", modelMatrix);
	m_pMaterialProperties->SetValue(name, "viewMatrix", cameraViewMatrix);
	m_pMaterialProperties->SetValue(name, "projMatrix", cameraProjMatrix);
	m_pMaterialProperties->SetValue(name, "normalMatrix", GetTransform()->GetNormalMatrix());
	m_pMaterialProperties->SetValue(name, "localToClipMatrix", localToClipMatrix);
}
void MeshRenderer::SetLightData(const std::array<DirectionalLight*, MAX_D_LIGHTS>& directionalLights)
{
	static std::string blockName = "LightData";
	static std::string arrayName = "directionalLightData";

	for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
		if (directionalLights[i] != nullptr)
		{
			Float4x4 worldToClipMatrix = directionalLights[i]->GetProjectionMatrix() * directionalLights[i]->GetViewMatrix();
			m_pMaterialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", worldToClipMatrix);
			m_pMaterialProperties->SetValue(blockName, arrayName, i, "direction", directionalLights[i]->GetDirection());
			m_pMaterialProperties->SetValue(blockName, arrayName, i, "colorIntensity", directionalLights[i]->GetColorIntensity());
		}
	m_pMaterialProperties->SetValue(blockName, "receiveShadows", m_receiveShadows);
}
void MeshRenderer::SetLightData(const std::array<SpotLight*, MAX_S_LIGHTS>& spotLights)
{
	static std::string blockName = "LightData";
	static std::string arrayName = "spotLightData";

	for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
		if (spotLights[i] != nullptr)
		{
			Float4x4 worldToClipMatrix = spotLights[i]->GetProjectionMatrix() * spotLights[i]->GetViewMatrix();
			m_pMaterialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", worldToClipMatrix);
			m_pMaterialProperties->SetValue(blockName, arrayName, i, "position", spotLights[i]->GetPosition());
			m_pMaterialProperties->SetValue(blockName, arrayName, i, "colorIntensity", spotLights[i]->GetColorIntensity());
			m_pMaterialProperties->SetValue(blockName, arrayName, i, "blendStartEnd", spotLights[i]->GetBlendStartEnd());
		}
	m_pMaterialProperties->SetValue(blockName, "receiveShadows", m_receiveShadows);
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
				m_pMaterialProperties->SetValue(blockName, arrayName, i, "worldToClipMatrix", faceIndex, worldToClipMatrix);
			}
			m_pMaterialProperties->SetValue(blockName, arrayName, i, "position", pointLights[i]->GetPosition());
			m_pMaterialProperties->SetValue(blockName, arrayName, i, "colorIntensity", pointLights[i]->GetColorIntensity());
		}
	m_pMaterialProperties->SetValue(blockName, "receiveShadows", m_receiveShadows);
}



// Shading render pass getters:
bool MeshRenderer::GetCastShadows() const
{
	// Always return false if error material is in use:
	return !m_hasErrorMaterial && m_castShadows;
}
bool MeshRenderer::GetReceiveShadows() const
{
	// Always return false if error material is in use:
	return !m_hasErrorMaterial && m_receiveShadows;
}
Mesh* MeshRenderer::GetMesh()
{
	return m_pMesh;
}
Material* MeshRenderer::GetMaterial()
{
	return m_pMaterial;
}
MaterialProperties* MeshRenderer::GetMaterialProperties()
{
	return m_pMaterialProperties.get();
}
const VkDescriptorSet* const MeshRenderer::GetShadingDescriptorSets(uint32_t frameIndex) const
{
	return &m_pMaterialProperties->GetDescriptorSets()[frameIndex];
}
const VkPipeline& MeshRenderer::GetShadingPipeline() const
{
	return m_pMaterial->GetPipeline()->GetVkPipeline();
}
const VkPipelineLayout& MeshRenderer::GetShadingPipelineLayout() const
{
	return m_pMaterial->GetPipeline()->GetVkPipelineLayout();
}

// Shadow render pass getters:
const VkDescriptorSet* const MeshRenderer::GetShadowDescriptorSets(uint32_t frameIndex)
{
	return &m_pShadowMaterialProperties->GetDescriptorSets()[frameIndex];
}
const VkPipeline& MeshRenderer::GetShadowPipeline()
{
	return m_pShadowMaterial->GetPipeline()->GetVkPipeline();
}
const VkPipelineLayout& MeshRenderer::GetShadowPipelineLayout()
{
	return m_pShadowMaterial->GetPipeline()->GetVkPipelineLayout();
}



// Overrides:
const std::string MeshRenderer::ToString() const
{
	return "MeshRenderer";
}