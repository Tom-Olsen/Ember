#include "graphics.h"
#include "computeShader.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "poolManager.h"
#include "shaderProperties.h"



namespace vulkanRendererBackend
{
	// Static members:
	bool Graphics::s_isInitialized = false;
	Graphics::Camera Graphics::s_activeCamera;
	std::vector<DrawCall> Graphics::s_staticDrawCalls;
	std::vector<DrawCall> Graphics::s_dynamicDrawCalls;
	std::vector<DrawCall*> Graphics::s_sortedDrawCallPointers;
	std::unique_ptr<Mesh> Graphics::s_pFullScreenRenderQuad;
	std::unique_ptr<Material> Graphics::s_pShadowMaterial;
	std::unique_ptr<Material> Graphics::s_pDefaultMaterial;
	std::unique_ptr<Material> Graphics::s_pErrorMaterial;
	std::unique_ptr<Material> Graphics::s_pPresentMaterial;
	std::unique_ptr<ComputeShader> Graphics::s_pGammaCorrectionComputeShader;
	float Graphics::s_depthBiasConstantFactor = 0.0f;
	float Graphics::s_depthBiasClamp = 0.0f;
	float Graphics::s_depthBiasSlopeFactor = 1.0f;



	// Initialization/Cleanup:
	void Graphics::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		std::filesystem::path shaderDir = std::filesystem::path(VULKAN_LIBRARY_ROOT_PATH) / "src" / "shaders";
        s_pFullScreenRenderQuad = std::unique_ptr<Mesh>(CreateFullScreenRenderQuad());
		s_pShadowMaterial = std::make_unique<Material>(Material::Type::shadow, "shadowMaterial", Material::Queue::shadow, shaderDir / "shadow.vert.spv");
		s_pDefaultMaterial = std::make_unique<Material>(Material::Type::forwardOpaque, "defaultMaterial", Material::Queue::opaque, shaderDir / "default.vert.spv", shaderDir / "default.frag.spv");
		s_pErrorMaterial = std::make_unique<Material>(Material::Type::forwardOpaque, "errorMaterial", Material::Queue::opaque, shaderDir / "error.vert.spv", shaderDir / "error.frag.spv");
		s_pPresentMaterial = std::make_unique<Material>(Material::Type::forwardOpaque, "presentMaterial", Material::Queue::opaque, shaderDir / "present.vert.spv", shaderDir / "present.frag.spv");
		s_pGammaCorrectionComputeShader = std::make_unique<ComputeShader>("gammaCorrectionComputeShader", shaderDir / "gammaCorrection.comp.spv");
	}
	void Graphics::Clear()
	{
		ResetDrawCalls();
        s_pFullScreenRenderQuad.release();
        s_pShadowMaterial.release();
        s_pDefaultMaterial.release();
        s_pErrorMaterial.release();
		s_pPresentMaterial.release();
		s_pGammaCorrectionComputeShader.release();
	}



	// Public methods:
	// Draw mesh:
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
		if (pMaterial == s_pErrorMaterial.get())
		{
			receiveShadows = false;
			castShadows = false;
		}

		// Setup draw call:
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)s_pShadowMaterial.get());
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
		if (pMaterial == s_pErrorMaterial.get())
		{
			receiveShadows = false;
			castShadows = false;
		}

		// Setup draw call:
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)pMaterial);
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)s_pShadowMaterial.get());
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
		if (pMaterial == s_pErrorMaterial.get())
		{
			receiveShadows = false;
			castShadows = false;
		}

		// Setup draw call:
		pShaderProperties->SetStorageBuffer("instanceBuffer", pInstanceBuffer);
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)s_pShadowMaterial.get());
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
		if (pMaterial == s_pErrorMaterial.get())
		{
			receiveShadows = false;
			castShadows = false;
		}

		// Setup draw call:
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)pMaterial);
		ShaderProperties* pShadowShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)s_pShadowMaterial.get());
		pShaderProperties->SetStorageBuffer("instanceBuffer", pInstanceBuffer);
		pShadowShaderProperties->SetStorageBuffer("instanceBuffer", pInstanceBuffer);
		DrawCall drawCall = { localToWorldMatrix, receiveShadows, castShadows, pMaterial, pShaderProperties, pShadowShaderProperties, pMesh, instanceCount };
		s_dynamicDrawCalls.push_back(drawCall);

		// By returning pShaderProperties, we allow user to change the shader properties of the draw call:
		return pShaderProperties;
	}



	// Setters:
	void Graphics::SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix)
	{
		s_activeCamera.position = position;
		s_activeCamera.viewMatrix = viewMatrix;
		s_activeCamera.projectionMatrix = projectionMatrix;
	}
	void Graphics::SetDepthBiasConstantFactor(float depthBiasConstantFactor)
	{
		s_depthBiasConstantFactor = depthBiasConstantFactor;
	}
	void Graphics::SetDepthBiasClamp(float depthBiasClamp)
	{
		s_depthBiasClamp = depthBiasClamp;
	}
	void Graphics::SetDepthBiasSlopeFactor(float depthBiasSlopeFactor)
	{
		s_depthBiasSlopeFactor = depthBiasSlopeFactor;
	}



	// Getters:
    Mesh* Graphics::GetFullScreenRenderQuad()
    {
        return s_pFullScreenRenderQuad.get();
    }
	Material* Graphics::GetShadowMaterial()
	{
		return s_pShadowMaterial.get();
	}
	Material* Graphics::GetDefaultMaterial()
	{
		return s_pDefaultMaterial.get();
	}
	Material* Graphics::GetPresentMaterial()
	{
		return s_pPresentMaterial.get();
	}
	ComputeShader* Graphics::GetGammaCorrectionComputeShader()
	{
		return s_pGammaCorrectionComputeShader.get();
	}
	const Graphics::Camera& Graphics::GetActiveCamera()
	{
		return s_activeCamera;
	}
	std::vector<DrawCall*>& Graphics::GetSortedDrawCallPointers()
	{
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
		return s_sortedDrawCallPointers;
	}
	float Graphics::GetDeptBiasConstantFactor()
	{
		return s_depthBiasConstantFactor;
	}
	float Graphics::GetDeptBiasClamp()
	{
		return s_depthBiasClamp;
	}
	float Graphics::GetDeptBiasSlopeFactor()
	{
		return s_depthBiasSlopeFactor;
	}



	// Management:
	void Graphics::ResetDrawCalls()
	{
		// Return all p(Shadow)ShaderProperties of dynamic draw calls back to the corresponding pool:
		for (DrawCall& drawCall : s_dynamicDrawCalls)
		{
			PoolManager::ReturnShaderProperties((Shader*)drawCall.pMaterial, drawCall.pShaderProperties);
			PoolManager::ReturnShaderProperties((Shader*)s_pShadowMaterial.get(), drawCall.pShadowShaderProperties);
		}

		// Return all pShadowShaderProperties of static draw calls back to the pool:
		for (DrawCall& drawCall : s_staticDrawCalls)
			PoolManager::ReturnShaderProperties((Shader*)s_pShadowMaterial.get(), drawCall.pShadowShaderProperties);

		// Remove all draw/compute calls so next frame can start fresh:
		s_staticDrawCalls.clear();
		s_dynamicDrawCalls.clear();
	}



    // Private methods:
	Mesh* Graphics::CreateFullScreenRenderQuad()
	{
		Mesh* pMesh = new Mesh("fullScreenRenderQuad");

		std::vector<Float3> positions;
		positions.emplace_back(-1.0f, -1.0f, 0.0f);
		positions.emplace_back(-1.0f,  1.0f, 0.0f);
		positions.emplace_back( 1.0f, -1.0f, 0.0f);
		positions.emplace_back( 1.0f,  1.0f, 0.0f);

		std::vector<Float3> normals;
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);
		normals.emplace_back(0.0f, 0.0f, 1.0f);

		std::vector<Float4> uvs;
		uvs.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(0.0f, 1.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 0.0f, 0.0f, 0.0f);
		uvs.emplace_back(1.0f, 1.0f, 0.0f, 0.0f);

		std::vector<Uint3> triangles;
		triangles.emplace_back(Uint3(0, 2, 1));
		triangles.emplace_back(Uint3(1, 2, 3));

		pMesh->MovePositions(positions);
		pMesh->MoveNormals(normals);
		pMesh->MoveUVs(uvs);
		pMesh->MoveTriangles(triangles);
		pMesh->ComputeTangents();
		return pMesh;
	}
}