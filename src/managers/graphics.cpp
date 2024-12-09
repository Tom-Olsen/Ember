#include "graphics.h"
#include "vulkanContext.h"
#include "vulkanRenderer.h"
#include "mesh.h"
#include "material.h"
#include "materialProperties.h"
#include "meshRenderer.h"
#include "transform.h"
#include "logger.h"



// Static members:
uint32_t Graphics::drawIndex = 0;
bool Graphics::isInitialized = false;
VulkanContext* Graphics::m_pContext = nullptr;
std::vector<MeshRenderer*> Graphics::m_meshRenderers;



// Initialization and cleanup:
void Graphics::Init(VulkanContext* pContext)
{
	if (isInitialized)
		return;

	isInitialized = true;
	m_pContext = pContext;

	m_meshRenderers.resize(100);
	for (uint32_t i = 0; i < 100; i++)
	{
		m_meshRenderers[i] = new MeshRenderer();
		m_meshRenderers[i]->transform = new Transform();
		m_meshRenderers[i]->isActive = false;
	}
}
void Graphics::Clear()
{
	for (MeshRenderer* pMeshRenderer : m_meshRenderers)
	{
		delete pMeshRenderer->transform;
		delete pMeshRenderer;
	}
	m_meshRenderers.clear();
}



// Public methods:
// Draw calls:
MaterialProperties* Graphics::Draw(Mesh* pMesh, Material* pMaterial, Float3 position, Float3x3 rotationMatrix, float scale, bool receiveShadows, bool castShadows)
{
	return Draw(pMesh, pMaterial, position, rotationMatrix, Float3(scale), receiveShadows, castShadows);
}
MaterialProperties* Graphics::Draw(Mesh* pMesh, Material* pMaterial, Float3 position, Float3x3 rotationMatrix, Float3 scale, bool receiveShadows, bool castShadows)
{
	Float4x4 translationMatrix = Float4x4::Translate(position);
	Float4x4 scaleMatrix = Float4x4::Scale(scale);
	Float4x4 localToWorldMatrix = translationMatrix * Float4x4(rotationMatrix) * scaleMatrix;
	return Draw(pMesh, pMaterial, localToWorldMatrix, receiveShadows, castShadows);
}
MaterialProperties* Graphics::Draw(Mesh* pMesh, Material* pMaterial, Float4x4 localToWorldMatrix, bool receiveShadows, bool castShadows)
{
	// Double size of the meshRenderers vector if it is full:
	if (drawIndex >= static_cast<uint32_t>(m_meshRenderers.size()))
	{
		uint32_t oldSize = static_cast<uint32_t>(m_meshRenderers.size());
		m_meshRenderers.resize(2 * m_meshRenderers.size());
		for (uint32_t i = oldSize; i < m_meshRenderers.size(); i++)
		{
			m_meshRenderers[i] = new MeshRenderer();
			m_meshRenderers[i]->transform = new Transform();
			m_meshRenderers[i]->isActive = false;
		}
	}

	// Setup current draw call:
	MeshRenderer* pMeshRenderer = m_meshRenderers[drawIndex];
	pMeshRenderer->isActive = true;
	pMeshRenderer->mesh = pMesh;
	if (pMeshRenderer->GetMaterial() != pMaterial)
		pMeshRenderer->SetMaterial(pMaterial);
	pMeshRenderer->castShadows = castShadows;
	pMeshRenderer->receiveShadows = receiveShadows;
	pMeshRenderer->transform->SetLocalToWorldMatrix(localToWorldMatrix);

	// By returning the meshRenderers materialProperties, we allow user to change the material properties of this draw call:
	MaterialProperties* pMaterialProperties = m_meshRenderers[drawIndex]->materialProperties.get();
	drawIndex++;
	return pMaterialProperties;
}
void Graphics::ResetDrawCalls()
{
	for (MeshRenderer* pMeshRenderer : m_meshRenderers)
		pMeshRenderer->isActive = false;
	drawIndex = 0;
}

// Getters:
std::vector<MeshRenderer*>* Graphics::GetSortedMeshRenderers()
{
	// Sort meshRenders according to material renderQueue:
	std::sort(m_meshRenderers.begin(), m_meshRenderers.end(), [](MeshRenderer* a, MeshRenderer* b)
	{
		// Handle nullptr cases for materials (MeshRenderer* not in use):
		const Material* materialA = a ? a->GetMaterial() : nullptr;
		const Material* materialB = b ? b->GetMaterial() : nullptr;

		// Default high value for nullptr:
		uint32_t renderQueueA = materialA ? static_cast<uint32_t>(materialA->GetRenderQueue()) : UINT32_MAX; 
		uint32_t renderQueueB = materialB ? static_cast<uint32_t>(materialB->GetRenderQueue()) : UINT32_MAX;

		return renderQueueA < renderQueueB;
	});
	return &m_meshRenderers;
}