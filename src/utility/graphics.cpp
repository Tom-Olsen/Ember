#include "graphics.h"
#include "material.h"
#include "materialManager.h"
#include "materialProperties.h"
#include "mesh.h"
#include "meshManager.h"
#include "meshRenderer.h"
#include "transform.h"



// Static members:
uint32_t Graphics::s_drawIndex = 0;
bool Graphics::s_isInitialized = false;
std::vector<Transform*> Graphics::s_transforms;
std::vector<MeshRenderer*> Graphics::s_meshRenderers;
Mesh* Graphics::s_pLineSegmentMesh;
Material* Graphics::s_pLineSegmentMaterial;



// Initialization/Cleanup:
void Graphics::Init()
{
	if (s_isInitialized)
		return;
	s_isInitialized = true;

	s_transforms.resize(100);
	s_meshRenderers.resize(100);
	for (uint32_t i = 0; i < 100; i++)
	{
		s_transforms[i] = new Transform();
		s_meshRenderers[i] = new MeshRenderer();
		s_meshRenderers[i]->SetTransform(s_transforms[i]);
		s_meshRenderers[i]->isActive = false;
	}
	s_pLineSegmentMesh = MeshManager::GetMesh("zylinderEdgy");
	s_pLineSegmentMaterial = MaterialManager::GetMaterial("simpleLit");
}
void Graphics::Clear()
{
	for (uint32_t i = 0; i < s_transforms.size(); i++)
	{
		delete s_transforms[i];
		delete s_meshRenderers[i];
	}
	s_transforms.clear();
	s_meshRenderers.clear();
}
void Graphics::SetLineSegmentMesh(Mesh* pMesh)
{
	if (pMesh != nullptr)
		s_pLineSegmentMesh = pMesh;
}



// Public methods:
// Draw mesh:
MaterialProperties* Graphics::DrawMesh(Mesh* pMesh, Material* pMaterial, Float3 position, Float3x3 rotationMatrix, float scale, bool receiveShadows, bool castShadows)
{
	return DrawMesh(pMesh, pMaterial, position, rotationMatrix, Float3(scale), receiveShadows, castShadows);
}
MaterialProperties* Graphics::DrawMesh(Mesh* pMesh, Material* pMaterial, Float3 position, Float3x3 rotationMatrix, Float3 scale, bool receiveShadows, bool castShadows)
{
	Float4x4 translationMatrix = Float4x4::Translate(position);
	Float4x4 scaleMatrix = Float4x4::Scale(scale);
	Float4x4 localToWorldMatrix = translationMatrix * Float4x4(rotationMatrix) * scaleMatrix;
	return DrawMesh(pMesh, pMaterial, localToWorldMatrix, receiveShadows, castShadows);
}
MaterialProperties* Graphics::DrawMesh(Mesh* pMesh, Material* pMaterial, Float4x4 localToWorldMatrix, bool receiveShadows, bool castShadows)
{
	if (!pMesh)
	{
		LOG_ERROR("Graphics::Draw() failed. pMesh is nullptr.");
		return nullptr;
	}

	DoubleCapacityIfNeeded();

	// Setup current draw call:
	MeshRenderer* pMeshRenderer = s_meshRenderers[s_drawIndex];
	pMeshRenderer->isActive = true;
	pMeshRenderer->SetMesh(pMesh);
	pMeshRenderer->SetMaterial(pMaterial);
	pMeshRenderer->SetCastShadows(castShadows);
	pMeshRenderer->SetReceiveShadows(receiveShadows);
	pMeshRenderer->GetTransform()->SetLocalToWorldMatrix(localToWorldMatrix);

	// By returning the meshRenderers materialProperties, we allow user to change the material properties of this draw call:
	MaterialProperties* pMaterialProperties = s_meshRenderers[s_drawIndex]->GetMaterialProperties();
	s_drawIndex++;
	return pMaterialProperties;
}

// Draw line segment:
void Graphics::DrawLineSegment(Float3 start, Float3 end, float width, Float4 color, bool receiveShadows, bool castShadows)
{
	MaterialProperties* pMaterialProperties = DrawLineSegment(start, end, width, s_pLineSegmentMaterial, receiveShadows, castShadows);
	pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", color);
}
MaterialProperties* Graphics::DrawLineSegment(Float3 start, Float3 end, float width, Material* pMaterial, bool receiveShadows, bool castShadows)
{
	Float3 direction = end - start;
	float length = direction.Length();
	Float3 position = start + 0.5f * direction;
	Float3x3 rotationMatrix = Float3x3::RotateFromTo(Float3::forward, direction);
	Float3 scale = Float3(width, width, length);
	return DrawMesh(s_pLineSegmentMesh, pMaterial, position, rotationMatrix, scale, receiveShadows, castShadows);
}

// Speciaized draw calls:
void Graphics::DrawFrustum(Transform* pTransform, const Float4x4& projectionMatrix, const Float4& color , float width, bool receiveShadows, bool castShadows)
{
	// Corner positions in normalized device coordinates:
	Float4 cornerPositions[8] =
	{
		Float4(-1, -1, -1, 1),
		Float4(-1, -1,  1, 1),
		Float4(-1,  1, -1, 1),
		Float4(-1,  1,  1, 1),
		Float4( 1, -1, -1, 1),
		Float4( 1, -1,  1, 1),
		Float4( 1,  1, -1, 1),
		Float4( 1,  1,  1, 1)
	};

	float det = projectionMatrix.Determinant();
	if (det == 0)
		return;

	Float4x4 invPerspectiveMatrix = projectionMatrix.Inverse(det);
	Float4x4 localToWorldMatrix = pTransform->GetLocalToWorldMatrix();

	for (uint32_t i = 0; i < 8; i++)
	{
		// Corner positions in camera/local space:
		cornerPositions[i] = invPerspectiveMatrix * cornerPositions[i];
		cornerPositions[i] /= cornerPositions[i].w;

		// Corner positions in world space:
		cornerPositions[i] = localToWorldMatrix * cornerPositions[i];
	}

	for (uint32_t i = 0; i < 8; i++)
	{
		MaterialProperties* pMaterialProperties = Graphics::DrawMesh(MeshManager::GetMesh("cubeSphere"), MaterialManager::GetMaterial("simpleLit"), Float3(cornerPositions[i]), Float3x3::identity, Float3(2.0f * width), receiveShadows, castShadows);
		pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", color);
	}

	// Horizontal lines:
	DrawLineSegment(Float3(cornerPositions[0]), Float3(cornerPositions[4]), width, 0.8f * color, receiveShadows, castShadows);
	DrawLineSegment(Float3(cornerPositions[1]), Float3(cornerPositions[5]), width, 0.8f * color, receiveShadows, castShadows);
	DrawLineSegment(Float3(cornerPositions[2]), Float3(cornerPositions[6]), width, 0.8f * color, receiveShadows, castShadows);
	DrawLineSegment(Float3(cornerPositions[3]), Float3(cornerPositions[7]), width, 0.8f * color, receiveShadows, castShadows);

	// Vertical lines:
	DrawLineSegment(Float3(cornerPositions[0]), Float3(cornerPositions[2]), width, 0.8f * color, receiveShadows, castShadows);
	DrawLineSegment(Float3(cornerPositions[1]), Float3(cornerPositions[3]), width, 0.8f * color, receiveShadows, castShadows);
	DrawLineSegment(Float3(cornerPositions[4]), Float3(cornerPositions[6]), width, 0.8f * color, receiveShadows, castShadows);
	DrawLineSegment(Float3(cornerPositions[5]), Float3(cornerPositions[7]), width, 0.8f * color, receiveShadows, castShadows);

	// Depth lines:
	DrawLineSegment(Float3(cornerPositions[0]), Float3(cornerPositions[1]), width, 0.8f * color, receiveShadows, castShadows);
	DrawLineSegment(Float3(cornerPositions[2]), Float3(cornerPositions[3]), width, 0.8f * color, receiveShadows, castShadows);
	DrawLineSegment(Float3(cornerPositions[4]), Float3(cornerPositions[5]), width, 0.8f * color, receiveShadows, castShadows);
	DrawLineSegment(Float3(cornerPositions[6]), Float3(cornerPositions[7]), width, 0.8f * color, receiveShadows, castShadows);
}



// ResetDrawCalls:
void Graphics::ResetDrawCalls()
{
	for (MeshRenderer* pMeshRenderer : s_meshRenderers)
		pMeshRenderer->isActive = false;
	ReduceCapacity();
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



// Private methods:
void Graphics::DoubleCapacityIfNeeded()
{
	uint32_t oldSize = static_cast<uint32_t>(s_transforms.size());
	if (s_drawIndex >= oldSize)
	{
		uint32_t newSize = 2 * oldSize;
		s_transforms.resize(newSize);
		s_meshRenderers.resize(newSize);
		for (uint32_t i = oldSize; i < newSize; i++)
		{
			s_transforms[i] = new Transform();
			s_meshRenderers[i] = new MeshRenderer();
			s_meshRenderers[i]->SetTransform(s_transforms[i]);
			s_meshRenderers[i]->isActive = false;
		}
	}
}
void Graphics::ReduceCapacity()
{
	uint32_t oldSize = static_cast<uint32_t>(s_transforms.size());
	uint32_t newSize = s_drawIndex; // drawIndex is one past the last active element, so no +1 needed.
	if (oldSize > newSize)
	{
		for (uint32_t i = newSize; i < oldSize; i++)
		{
			delete s_transforms[i];
			delete s_meshRenderers[i];
		}
		s_transforms.resize(newSize);
		s_meshRenderers.resize(newSize);
	}
}