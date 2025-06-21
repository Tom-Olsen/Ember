#include "graphics.h"
#include "computeShader.h"
#include "computeShaderManager.h"
#include "logger.h"
#include "macros.h"
#include "material.h"
#include "materialManager.h"
#include "mesh.h"
#include "meshManager.h"
#include "poolManager.h"
#include "profiler.h"
#include "renderPassManager.h"
#include "renderTexture2d.h"
#include "shader.h"
#include "shaderProperties.h"
#include "shaderPropertiesPool.h"
#include "vulkanForwardRenderPass.h"



namespace emberEngine
{
	// Static members:
	bool Graphics::s_isInitialized = false;
	Graphics::Camera Graphics::s_activeCamera;
	Material* Graphics::s_pShadowMaterial;
	std::vector<DrawCall> Graphics::s_staticDrawCalls;
	std::vector<DrawCall> Graphics::s_dynamicDrawCalls;
	std::vector<DrawCall*> Graphics::s_sortedDrawCallPointers;
	Mesh* Graphics::s_pLineSegmentMesh;
	Mesh* Graphics::s_pSphereMesh;
	Mesh* Graphics::s_pArrowMesh;
	Material* Graphics::s_pSimpleLitMaterial;
	Material* Graphics::s_pSimpleUnlitMaterial;
	Material* Graphics::s_errorMaterial;



	// Initialization/Cleanup:
	void Graphics::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_pShadowMaterial = MaterialManager::GetMaterial("shadowMaterial");
		s_pLineSegmentMesh = MeshManager::GetMesh("zylinderEdgy");
		s_pSphereMesh = MeshManager::GetMesh("cubeSphere");
		s_pArrowMesh = MeshManager::GetMesh("arrowEdgy");
		s_pSimpleLitMaterial = MaterialManager::GetMaterial("simpleLitMaterial");
		s_pSimpleUnlitMaterial = MaterialManager::GetMaterial("simpleUnlitMaterial");
		s_errorMaterial = MaterialManager::GetMaterial("errorMaterial");

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("Graphics initialized.");
		#endif
	}
	void Graphics::Clear()
	{
		ResetDrawCalls();
	}



	// Setters:
	void Graphics::SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix)
	{
		s_activeCamera.position = position;
		s_activeCamera.viewMatrix = viewMatrix;
		s_activeCamera.projectionMatrix = projectionMatrix;
	}
	void Graphics::SetLineSegmentMesh(Mesh* pMesh)
	{
		if (pMesh != nullptr)
			s_pLineSegmentMesh = pMesh;
	}



	// Public methods:
	// Draw mesh:
	ShaderProperties* Graphics::DrawMesh(Mesh* pMesh, Material* pMaterial, const Float3& position, const Float3x3& rotationMatrix, float scale, bool receiveShadows, bool castShadows)
	{
		return DrawMesh(pMesh, pMaterial, position, rotationMatrix, Float3(scale), receiveShadows, castShadows);
	}
	ShaderProperties* Graphics::DrawMesh(Mesh* pMesh, Material* pMaterial, const Float3& position, const Float3x3& rotationMatrix, const Float3& scale, bool receiveShadows, bool castShadows)
	{
		Float4x4 localToWorldMatrix = Float4x4::TRS(position, rotationMatrix, scale);
		return DrawMesh(pMesh, pMaterial, localToWorldMatrix, receiveShadows, castShadows);
	}
	void Graphics::DrawMesh(Mesh* pMesh, Material* pMaterial, ShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		if (!pMesh)
		{
			LOG_ERROR("Graphics::DrawMesh(...) failed. pMesh is nullptr.");
			return;
		}
		if (!pMaterial)
		{
			LOG_ERROR("Graphics::DrawMesh(...) failed. pMaterial is nullptr.");
			return;
		}

		// No shadow interaction for the error material:
		if (pMaterial == s_errorMaterial)
		{
			receiveShadows = false;
			castShadows = false;
		}

		// Setup draw call:
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)s_pShadowMaterial);
		DrawCall drawCall = { localToWorldMatrix, receiveShadows, castShadows, pMaterial, pShaderProperties, pShadowShaderProperties, pMesh, 0 };
		s_staticDrawCalls.push_back(drawCall);
	}
	ShaderProperties* Graphics::DrawMesh(Mesh* pMesh, Material* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		if (!pMesh)
		{
			LOG_ERROR("Graphics::DrawMesh(...) failed. pMesh is nullptr.");
			return nullptr;
		}
		if (!pMaterial)
		{
			LOG_ERROR("Graphics::DrawMesh(...) failed. pMaterial is nullptr.");
			return nullptr;
		}

		// No shadow interaction for the error material:
		if (pMaterial == s_errorMaterial)
		{
			receiveShadows = false;
			castShadows = false;
		}

		// Setup draw call:
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)pMaterial);
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)s_pShadowMaterial);
		DrawCall drawCall = { localToWorldMatrix, receiveShadows, castShadows, pMaterial, pShaderProperties, pShadowShaderProperties, pMesh, 0 };
		s_dynamicDrawCalls.push_back(drawCall);

		// By returning pShaderProperties, we allow user to change the shader properties of the draw call:
		return pShaderProperties;
	}

	// Draw instanced:
	void Graphics::DrawInstanced(uint32_t instanceCount, StorageBuffer* pInstanceBuffer, Mesh* pMesh, Material* pMaterial, ShaderProperties* pShaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		if (!pInstanceBuffer)
		{
			LOG_ERROR("Graphics::DrawInstanced(...) failed. pInstanceBuffer is nullptr.");
			return;
		}
		if (!pMesh)
		{
			LOG_ERROR("Graphics::DrawInstanced(...) failed. pMesh is nullptr.");
			return;
		}
		if (!pMaterial)
		{
			LOG_ERROR("Graphics::DrawInstanced(...) failed. pMaterial is nullptr.");
			return;
		}

		// No shadow interaction for the error material:
		if (pMaterial == s_errorMaterial)
		{
			receiveShadows = false;
			castShadows = false;
		}

		// Setup draw call:
		pShaderProperties->SetStorageBuffer("instanceBuffer", pInstanceBuffer);
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)s_pShadowMaterial);
		pShadowShaderProperties->SetStorageBuffer("instanceBuffer", pInstanceBuffer);
		DrawCall drawCall = { localToWorldMatrix, receiveShadows, castShadows, pMaterial, pShaderProperties, pShadowShaderProperties, pMesh, instanceCount };
		s_staticDrawCalls.push_back(drawCall);
	}
	ShaderProperties* Graphics::DrawInstanced(uint32_t instanceCount, StorageBuffer* pInstanceBuffer, Mesh* pMesh, Material* pMaterial, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows)
	{
		if (!pInstanceBuffer)
		{
			LOG_ERROR("Graphics::DrawInstanced(...) failed. pInstanceBuffer is nullptr.");
			return nullptr;
		}
		if (!pMesh)
		{
			LOG_ERROR("Graphics::DrawMesh(...) failed. pMesh is nullptr.");
			return nullptr;
		}
		if (!pMaterial)
		{
			LOG_ERROR("Graphics::DrawMesh(...) failed. pMaterial is nullptr.");
			return nullptr;
		}

		// No shadow interaction for the error material:
		if (pMaterial == s_errorMaterial)
		{
			receiveShadows = false;
			castShadows = false;
		}

		// Setup draw call:
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)pMaterial);
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)s_pShadowMaterial);
		pShaderProperties->SetStorageBuffer("instanceBuffer", pInstanceBuffer);
		pShadowShaderProperties->SetStorageBuffer("instanceBuffer", pInstanceBuffer);
		DrawCall drawCall = { localToWorldMatrix, receiveShadows, castShadows, pMaterial, pShaderProperties, pShadowShaderProperties, pMesh, instanceCount };
		s_dynamicDrawCalls.push_back(drawCall);

		// By returning pShaderProperties, we allow user to change the shader properties of the draw call:
		return pShaderProperties;
	}

	// Draw line segment:
	void Graphics::DrawLineSegment(const Float3& start, const Float3& end, float width, const Float4& color, bool receiveShadows, bool castShadows)
	{
		ShaderProperties* pShaderProperties = DrawLineSegment(start, end, width, s_pSimpleUnlitMaterial, receiveShadows, castShadows);
		pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", color);
	}
	ShaderProperties* Graphics::DrawLineSegment(const Float3& start, const Float3& end, float width, Material* pMaterial, bool receiveShadows, bool castShadows)
	{
		Float3 direction = end - start;
		float length = direction.Length();
		Float3 position = start + 0.5f * direction;
		Float3x3 rotationMatrix = Float3x3::RotateFromTo(Float3::up, direction);
		Float3 scale = Float3(width, width, length);
		return DrawMesh(s_pLineSegmentMesh, pMaterial, position, rotationMatrix, scale, receiveShadows, castShadows);
	}

	// Speciaized draw calls:
	void Graphics::DrawSphere(const Float3& position, float radius, const Float4& color, bool receiveShadows, bool castShadows)
	{
		ShaderProperties* pShaderProperties = DrawMesh(s_pSphereMesh, s_pSimpleUnlitMaterial, position, Float3x3::identity, Float3(radius), receiveShadows, castShadows);
		pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", color);
	}
	void Graphics::DrawArrow(const Float3& position, const Float3& direction, float size, const Float4& color, bool receiveShadows, bool castShadows)
	{
		Float3x3 rotationMatrix = Float3x3::RotateFromTo(Float3::forward, direction);
		ShaderProperties* pShaderProperties = DrawMesh(s_pArrowMesh, s_pSimpleUnlitMaterial, position, rotationMatrix, Float3(size), receiveShadows, castShadows);
		pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", color);
	}
	void Graphics::DrawFrustum(const Float4x4& localToWorldMatrix, const Float4x4& projectionMatrix, float width, const Float4& color, bool receiveShadows, bool castShadows)
	{
		// Corner positions in normalized device coordinates:
		Float4 cornerPoints[8] =
		{
			Float4(-1, -1, 0, 1),
			Float4(-1, -1, 1, 1),
			Float4(-1,  1, 0, 1),
			Float4(-1,  1, 1, 1),
			Float4(1, -1, 0, 1),
			Float4(1, -1, 1, 1),
			Float4(1,  1, 0, 1),
			Float4(1,  1, 1, 1)
		};

		float det = projectionMatrix.Determinant();
		if (det == 0)
			return;

		Float4x4 invPerspectiveMatrix = projectionMatrix.Inverse(det);
		for (uint32_t i = 0; i < 8; i++)
		{
			// Corner positions in camera/local space:
			cornerPoints[i] = invPerspectiveMatrix * cornerPoints[i];
			cornerPoints[i] /= cornerPoints[i].w;

			// Corner positions in world space:
			cornerPoints[i] = localToWorldMatrix * cornerPoints[i];
		}

		// Draw corner points:
		for (uint32_t i = 0; i < 8; i++)
		{
			ShaderProperties* pShaderProperties = Graphics::DrawMesh(MeshManager::GetMesh("cubeSphere"), s_pSimpleUnlitMaterial, Float3(cornerPoints[i]), Float3x3::identity, Float3(2.0f * width), receiveShadows, castShadows);
			pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::gray);
		}

		// Draw horizontal lines:
		DrawLineSegment(Float3(cornerPoints[0]), Float3(cornerPoints[4]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[1]), Float3(cornerPoints[5]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[2]), Float3(cornerPoints[6]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[3]), Float3(cornerPoints[7]), width, color, receiveShadows, castShadows);
		// Draw vertical lines:
		DrawLineSegment(Float3(cornerPoints[0]), Float3(cornerPoints[2]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[1]), Float3(cornerPoints[3]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[4]), Float3(cornerPoints[6]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[5]), Float3(cornerPoints[7]), width, color, receiveShadows, castShadows);
		// Draw depth lines:
		DrawLineSegment(Float3(cornerPoints[0]), Float3(cornerPoints[1]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[2]), Float3(cornerPoints[3]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[4]), Float3(cornerPoints[5]), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(cornerPoints[6]), Float3(cornerPoints[7]), width, color, receiveShadows, castShadows);
	}
	void Graphics::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds2d& bounds, float width, const Float4& color, bool receiveShadows, bool castShadows)
	{
		std::array<Float2, 4> cornerPoints = bounds.GetCorners();

		// Draw corner points:
		for (uint32_t i = 0; i < 4; i++)
		{
			Float3 pos = Float3(localToWorldMatrix * Float4(cornerPoints[i], 0.0f, 1.0f));
			ShaderProperties* pShaderProperties = Graphics::DrawMesh(MeshManager::GetMesh("cubeSphere"), s_pSimpleUnlitMaterial, pos, Float3x3::identity, Float3(2.0f * width), receiveShadows, castShadows);
			pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::black);
		}

		// Draw lines:
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[0], 0.0f, 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[1], 0.0f, 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[1], 0.0f, 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[3], 0.0f, 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[3], 0.0f, 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[2], 0.0f, 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[2], 0.0f, 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[0], 0.0f, 1.0f)), width, color, receiveShadows, castShadows);
	}
	void Graphics::DrawBounds(const Float4x4& localToWorldMatrix, const Bounds& bounds, float width, const Float4& color, bool receiveShadows, bool castShadows)
	{
		std::array<Float3, 8> cornerPoints = bounds.GetCorners();

		// Draw corner points:
		for (uint32_t i = 0; i < 8; i++)
		{
			Float3 pos = Float3(localToWorldMatrix * Float4(cornerPoints[i], 1.0f));
			ShaderProperties* pShaderProperties = Graphics::DrawMesh(MeshManager::GetMesh("cubeSphere"), s_pSimpleUnlitMaterial, pos, Float3x3::identity, Float3(2.0f * width), receiveShadows, castShadows);
			pShaderProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::black);
		}

		// Draw horizontal lines:
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[0], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[4], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[1], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[5], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[2], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[6], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[3], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[7], 1.0f)), width, color, receiveShadows, castShadows);
		// Draw vertical lines:
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[0], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[2], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[1], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[3], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[4], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[6], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[5], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[7], 1.0f)), width, color, receiveShadows, castShadows);
		// Draw depth lines:
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[0], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[1], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[2], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[3], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[4], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[5], 1.0f)), width, color, receiveShadows, castShadows);
		DrawLineSegment(Float3(localToWorldMatrix * Float4(cornerPoints[6], 1.0f)), Float3(localToWorldMatrix * Float4(cornerPoints[7], 1.0f)), width, color, receiveShadows, castShadows);
	}


	// Getters:
	const Graphics::Camera& Graphics::GetActiveCamera()
	{
		return s_activeCamera;
	}
	std::vector<DrawCall*>* Graphics::GetSortedDrawCallPointers()
	{
		PROFILE_SCOPE("GetSortedDrawCallPointers");
		// Populate sorted draw call pointers vector:
		s_sortedDrawCallPointers.clear();
		s_sortedDrawCallPointers.reserve(s_staticDrawCalls.size() + s_dynamicDrawCalls.size());
		for (auto& drawCall : s_staticDrawCalls)
			s_sortedDrawCallPointers.push_back(&drawCall);
		for (auto& drawCall : s_dynamicDrawCalls)
			s_sortedDrawCallPointers.push_back(&drawCall);

		// Sort draw call pointers according to material renderQueue:
		std::sort(s_sortedDrawCallPointers.begin(), s_sortedDrawCallPointers.end(), [](DrawCall* a, DrawCall* b)
		{
			return a->pMaterial->GetRenderQueue() < b->pMaterial->GetRenderQueue();
		});
		return &s_sortedDrawCallPointers;
	}



	// Management:
	void Graphics::ResetDrawCalls()
	{
		// Return all p(Shadow)ShaderProperties of dynamic draw calls back to the corresponding pool:
		for (DrawCall& drawCall : s_dynamicDrawCalls)
		{
			PoolManager::ReturnShaderProperties((Shader*)drawCall.pMaterial, drawCall.pShaderProperties);
			PoolManager::ReturnShaderProperties((Shader*)s_pShadowMaterial, drawCall.pShadowShaderProperties);
		}

		// Return all pShadowShaderProperties of static draw calls back to the pool:
		for (DrawCall& drawCall : s_staticDrawCalls)
			PoolManager::ReturnShaderProperties((Shader*)s_pShadowMaterial, drawCall.pShadowShaderProperties);

		// Remove all draw/compute calls so next frame can start fresh:
		s_staticDrawCalls.clear();
		s_dynamicDrawCalls.clear();
	}
}