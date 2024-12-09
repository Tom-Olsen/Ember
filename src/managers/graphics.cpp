#include "graphics.h"
#include "material.h"
#include "materialProperties.h"
#include "mesh.h"
#include "meshRenderer.h"
#include "transform.h"



// Static members:
uint32_t Graphics::s_drawIndex = 0;
bool Graphics::s_isInitialized = false;
std::vector<MeshRenderer*> Graphics::s_meshRenderers;



// Initialization/Cleanup:
void Graphics::Init()
{
	if (s_isInitialized)
		return;

	s_isInitialized = true;

	s_meshRenderers.resize(100);
	for (uint32_t i = 0; i < 100; i++)
	{
		s_meshRenderers[i] = new MeshRenderer();
		s_meshRenderers[i]->transform = new Transform();
		s_meshRenderers[i]->isActive = false;
	}
}
void Graphics::Clear()
{
	for (MeshRenderer* pMeshRenderer : s_meshRenderers)
	{
		delete pMeshRenderer->transform;
		delete pMeshRenderer;
	}
	s_meshRenderers.clear();
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
	if (s_drawIndex >= static_cast<uint32_t>(s_meshRenderers.size()))
	{
		uint32_t oldSize = static_cast<uint32_t>(s_meshRenderers.size());
		s_meshRenderers.resize(2 * s_meshRenderers.size());
		for (uint32_t i = oldSize; i < s_meshRenderers.size(); i++)
		{
			s_meshRenderers[i] = new MeshRenderer();
			s_meshRenderers[i]->transform = new Transform();
			s_meshRenderers[i]->isActive = false;
		}
	}

	// Setup current draw call:
	MeshRenderer* pMeshRenderer = s_meshRenderers[s_drawIndex];
	pMeshRenderer->isActive = true;
	pMeshRenderer->mesh = pMesh;
	if (pMeshRenderer->GetMaterial() != pMaterial)
		pMeshRenderer->SetMaterial(pMaterial);
	pMeshRenderer->castShadows = castShadows;
	pMeshRenderer->receiveShadows = receiveShadows;
	pMeshRenderer->transform->SetLocalToWorldMatrix(localToWorldMatrix);

	// By returning the meshRenderers materialProperties, we allow user to change the material properties of this draw call:
	MaterialProperties* pMaterialProperties = s_meshRenderers[s_drawIndex]->materialProperties.get();
	s_drawIndex++;
	return pMaterialProperties;
}
void Graphics::ResetDrawCalls()
{
	for (MeshRenderer* pMeshRenderer : s_meshRenderers)
		pMeshRenderer->isActive = false;
	s_drawIndex = 0;
}

// Getters:
std::vector<MeshRenderer*>* Graphics::GetSortedMeshRenderers()
{
	// Sort meshRenders according to material renderQueue:
	std::sort(s_meshRenderers.begin(), s_meshRenderers.end(), [](MeshRenderer* a, MeshRenderer* b)
	{
		// Handle nullptr cases for materials (MeshRenderer* not in use):
		const Material* materialA = a ? a->GetMaterial() : nullptr;
		const Material* materialB = b ? b->GetMaterial() : nullptr;

		// Default high value for nullptr:
		uint32_t renderQueueA = materialA ? static_cast<uint32_t>(materialA->GetRenderQueue()) : UINT32_MAX; 
		uint32_t renderQueueB = materialB ? static_cast<uint32_t>(materialB->GetRenderQueue()) : UINT32_MAX;

		return renderQueueA < renderQueueB;
	});
	return &s_meshRenderers;
}