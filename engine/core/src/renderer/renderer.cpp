#include "renderer.h"
#include "buffer.h"
#include "emberTime.h"
#include "iMaterial.h"
#include "iRenderer.h"
#include "logger.h"
#include "material.h"
#include "materialManager.h"
#include "materialShaderManager.h"
#include "mesh.h"
#include "shaderProperties.h"
#include "shadowMaterial.h"
#include "texture.h"
#include "texture2d.h"
#include "window.h"
#include <stdexcept>



namespace emberCore
{
	// Static members:
	bool Renderer::s_isInitialized = false;
	std::unique_ptr<emberBackendInterface::IRenderer> Renderer::s_pIRenderer;
	std::array<Float4x4, 6> Renderer::s_pointLightRotationMatrices;
	emberBackendInterface::IRenderer* Renderer::GetInterfaceHandle()
	{
		return s_pIRenderer.get();
	}



	// Public Methods:
	// Initialization/Cleanup:
	void Renderer::Init(emberBackendInterface::IRenderer* pIRenderer)
	{
		if (s_isInitialized)
			return;
		if (pIRenderer == nullptr)
			throw std::runtime_error("Renderer::Init(...) failed. pIRenderer is nullptr.");

		s_pointLightRotationMatrices[0] = Float4x4::identity;
		s_pointLightRotationMatrices[1] = Float4x4::RotateY( math::pi2);
		s_pointLightRotationMatrices[2] = Float4x4::RotateY( math::pi );
		s_pointLightRotationMatrices[3] = Float4x4::RotateY(-math::pi2);
		s_pointLightRotationMatrices[4] = Float4x4::RotateX( math::pi2);
		s_pointLightRotationMatrices[5] = Float4x4::RotateX(-math::pi2);

		s_pIRenderer = std::unique_ptr<emberBackendInterface::IRenderer>(pIRenderer);
		MaterialShaderManager::Init();
		MaterialManager::Init();
		s_isInitialized = true;
	}
	void Renderer::Clear()
	{
		MaterialManager::Clear();
		MaterialShaderManager::Clear();
		s_pIRenderer.reset();
		s_isInitialized = false;
	}



	// Main render loop:
	void Renderer::RenderFrame()
	{
		s_pIRenderer->RenderFrame(Time::GetTime(), Time::GetDeltaTime());
	}



	// Lightsources:
	void Renderer::AddDirectionalLight(const Float3& direction, float intensity, const Float3& color, emberCommon::ShadowType shadowType, const Float4x4& worldToClipMatrix)
	{
		s_pIRenderer->AddDirectionalLight(direction, intensity, color, shadowType, worldToClipMatrix);
	}
	void Renderer::AddPositionalLight(const Float3& position, float intensity, const Float3& color, emberCommon::ShadowType shadowType, float blendStart, float blendEnd, const Float4x4& worldToClipMatrix)
	{
		s_pIRenderer->AddPositionalLight(position, intensity, color, shadowType, blendStart, blendEnd, worldToClipMatrix);
	}



	// Draw mesh:
    void Renderer::DrawOutline(const Float4x4& localToWorldMatrix, const Mesh& mesh)
    {
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
        s_pIRenderer->DrawOutline(localToWorldMatrix, pIMesh, 0);
    }
	void Renderer::DrawMesh(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, bool receiveShadows, bool castShadows)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (!pICallDescriptorSetBinding)
		{
			LOG_WARN("Renderer::DrawMesh(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another draw call.");
			return;
		}
		s_pIRenderer->DrawMesh(localToWorldMatrix, pIMesh, pIMaterial, pICallDescriptorSetBinding, receiveShadows, 0);
		if (castShadows)
			DrawMeshShadow(localToWorldMatrix, mesh, MaterialManager::GetShadowMaterialForSurfaceMaterial(material.m_materialId));
	}
	ShaderProperties Renderer::DrawMesh(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material, bool receiveShadows, bool castShadows)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawMesh(localToWorldMatrix, pIMesh, pIMaterial, receiveShadows, 0);
		if (castShadows)
			DrawMeshShadow(localToWorldMatrix, mesh, MaterialManager::GetShadowMaterialForSurfaceMaterial(material.m_materialId));
		return ShaderProperties(pICallDescriptorSetBinding);
	}
	void Renderer::DrawMeshShadow(const Float4x4& localToWorldMatrix, const Mesh& mesh, const ShadowMaterial& material, ShaderProperties& shaderProperties)
	{
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("Renderer::DrawMeshShadow(...) failed. Shadow material is invalid or expired.");
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (pICallDescriptorSetBinding == nullptr)
		{
			LOG_WARN("Renderer::DrawMeshShadow(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another draw call.");
			return;
		}
		s_pIRenderer->DrawMeshShadow(localToWorldMatrix, pIMesh, pIMaterial, pICallDescriptorSetBinding, 0);
	}
	ShaderProperties Renderer::DrawMeshShadow(const Float4x4& localToWorldMatrix, const Mesh& mesh, const ShadowMaterial& material)
	{
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("Renderer::DrawMeshShadow(...) failed. Shadow material is invalid or expired.");
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawMeshShadow(localToWorldMatrix, pIMesh, pIMaterial, 0);
		return ShaderProperties(pICallDescriptorSetBinding);
	}
	void Renderer::DrawGizmo(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties)
	{
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (!pICallDescriptorSetBinding)
		{
			LOG_WARN("Renderer::DrawGizmo(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another draw call.");
			return;
		}
		s_pIRenderer->DrawGizmo(localToWorldMatrix, pIMesh, pIMaterial, pICallDescriptorSetBinding, 0);
	}
	ShaderProperties Renderer::DrawGizmo(const Float4x4& localToWorldMatrix, const Mesh& mesh, const Material& material)
	{
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawGizmo(localToWorldMatrix, pIMesh, pIMaterial, 0);
		return ShaderProperties(pICallDescriptorSetBinding);
	}



	// Draw instanced:
    void Renderer::DrawOutlineInstanced(const Float4x4& localToWorldMatrix, const Mesh& mesh, uint32_t instanceCount)
    {
        LOG_WARN("Renderer::DrawOutlineInstanced not supported yet!");
		//emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
        //s_pIRenderer->DrawOutline(localToWorldMatrix, pIMesh, instanceCount);
    }
	void Renderer::DrawMeshInstanced(const Float4x4& localToWorldMatrix, uint32_t instanceCount, Buffer& instanceBuffer, const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, bool receiveShadows, bool castShadows)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (!pICallDescriptorSetBinding)
		{
			LOG_WARN("Renderer::DrawMeshInstanced(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another draw call.");
			return;
		}
		if (shaderProperties.HasBinding("instanceBuffer"))
			shaderProperties.SetBuffer("instanceBuffer", instanceBuffer);
		s_pIRenderer->DrawMesh(localToWorldMatrix, pIMesh, pIMaterial, pICallDescriptorSetBinding, receiveShadows, instanceCount);
		if (castShadows)
			DrawMeshShadowInstanced(localToWorldMatrix, instanceCount, instanceBuffer, mesh, MaterialManager::GetShadowMaterialForSurfaceMaterial(material.m_materialId));
	}
	ShaderProperties Renderer::DrawMeshInstanced(const Float4x4& localToWorldMatrix, uint32_t instanceCount, Buffer& instanceBuffer, const Mesh& mesh, const Material& material, bool receiveShadows, bool castShadows)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawMesh(localToWorldMatrix, pIMesh, pIMaterial, receiveShadows, instanceCount);
		ShaderProperties shaderProperties(pICallDescriptorSetBinding);
		if (shaderProperties.HasBinding("instanceBuffer"))
			shaderProperties.SetBuffer("instanceBuffer", instanceBuffer);
		if (castShadows)
			DrawMeshShadowInstanced(localToWorldMatrix, instanceCount, instanceBuffer, mesh, MaterialManager::GetShadowMaterialForSurfaceMaterial(material.m_materialId));
		return shaderProperties;
	}
	void Renderer::DrawMeshInstanced(const Float4x4& localToWorldMatrix, uint32_t instanceCount, const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, bool receiveShadows, bool castShadows)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (!pICallDescriptorSetBinding)
		{
			LOG_WARN("Renderer::DrawMeshInstanced(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another draw call.");
			return;
		}
		s_pIRenderer->DrawMesh(localToWorldMatrix, pIMesh, pIMaterial, pICallDescriptorSetBinding, receiveShadows, instanceCount);
		if (castShadows)
			DrawMeshShadowInstanced(localToWorldMatrix, instanceCount, mesh, MaterialManager::GetShadowMaterialForSurfaceMaterial(material.m_materialId));
	}
	ShaderProperties Renderer::DrawMeshInstanced(const Float4x4& localToWorldMatrix, uint32_t instanceCount, const Mesh& mesh, const Material& material, bool receiveShadows, bool castShadows)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawMesh(localToWorldMatrix, pIMesh, pIMaterial, receiveShadows, instanceCount);
		if (castShadows)
			DrawMeshShadowInstanced(localToWorldMatrix, instanceCount, mesh, MaterialManager::GetShadowMaterialForSurfaceMaterial(material.m_materialId));
		return ShaderProperties(pICallDescriptorSetBinding);
	}
	void Renderer::DrawMeshShadowInstanced(const Float4x4& localToWorldMatrix, uint32_t instanceCount, Buffer& instanceBuffer, const Mesh& mesh, const ShadowMaterial& material, ShaderProperties& shaderProperties)
	{
		DrawMeshShadowInstanced(localToWorldMatrix, instanceCount, mesh, material, shaderProperties);
		if (shaderProperties.HasBinding("instanceBuffer"))
			shaderProperties.SetBuffer("instanceBuffer", instanceBuffer);
	}
	ShaderProperties Renderer::DrawMeshShadowInstanced(const Float4x4& localToWorldMatrix, uint32_t instanceCount, Buffer& instanceBuffer, const Mesh& mesh, const ShadowMaterial& material)
	{
		ShaderProperties shaderProperties = DrawMeshShadowInstanced(localToWorldMatrix, instanceCount, mesh, material);
		if (shaderProperties.HasBinding("instanceBuffer"))
			shaderProperties.SetBuffer("instanceBuffer", instanceBuffer);
		return shaderProperties;
	}
	void Renderer::DrawMeshShadowInstanced(const Float4x4& localToWorldMatrix, uint32_t instanceCount, const Mesh& mesh, const ShadowMaterial& material, ShaderProperties& shaderProperties)
	{
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("Renderer::DrawMeshShadowInstanced(...) failed. Shadow material is invalid or expired.");
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (pICallDescriptorSetBinding == nullptr)
		{
			LOG_WARN("Renderer::DrawMeshShadowInstanced(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another draw call.");
			return;
		}
		s_pIRenderer->DrawMeshShadow(localToWorldMatrix, pIMesh, pIMaterial, pICallDescriptorSetBinding, instanceCount);
	}
	ShaderProperties Renderer::DrawMeshShadowInstanced(const Float4x4& localToWorldMatrix, uint32_t instanceCount, const Mesh& mesh, const ShadowMaterial& material)
	{
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("Renderer::DrawMeshShadowInstanced(...) failed. Shadow material is invalid or expired.");
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawMeshShadow(localToWorldMatrix, pIMesh, pIMaterial, instanceCount);
		return ShaderProperties(pICallDescriptorSetBinding);
	}



	// Getters:
	bool Renderer::TryGetDirectionalLight(emberCommon::DirectionalLight& directionalLight, uint32_t index)
	{
		return s_pIRenderer->TryGetDirectionalLight(directionalLight, index);
	}
	bool Renderer::TryGetPositionalLight(emberCommon::PositionalLight& positionalLight, uint32_t index)
	{
		return s_pIRenderer->TryGetPositionalLight(positionalLight, index);
	}
	const uint32_t Renderer::GetShadowMapResolution()
	{
		return s_pIRenderer->GetShadowMapResolution();
	}
	const Uint2 Renderer::GetSurfaceExtent()
	{
		return s_pIRenderer->GetSurfaceExtent();
	}
	const Float4x4& Renderer::GetPointLightRotationMatrix(int faceIndex)
	{
		return s_pointLightRotationMatrices[faceIndex];
	}
	Texture2d Renderer::GetRenderTexture()
	{
		return Texture2d(s_pIRenderer->GetRenderTexture(), false);
	}
	Texture2d Renderer::GetGizmoTexture()
	{
		return Texture2d(s_pIRenderer->GetGizmoTexture(), false);
	}
	float Renderer::GetDepthBiasConstantFactor()
	{
		return s_pIRenderer->GetDepthBiasConstantFactor();
	}
	float Renderer::GetDepthBiasClamp()
	{
		return s_pIRenderer->GetDepthBiasClamp();
	}
	float Renderer::GetDepthBiasSlopeFactor()
	{
		return s_pIRenderer->GetDepthBiasSlopeFactor();
	}
	const Float4& Renderer::GetOutlineColor()
	{
		return s_pIRenderer->GetOutlineColor();
	}
	int Renderer::GetOutlineThickness()
	{
		return s_pIRenderer->GetOutlineThickness();
	}
	uint32_t Renderer::GetFrameIndex()
	{
		return s_pIRenderer ? s_pIRenderer->GetFrameIndex() : 0;
	}
	bool Renderer::IsFrameFinished(uint32_t frameIndex)
	{
		return s_pIRenderer ? s_pIRenderer->IsFrameFinished(frameIndex) : true;
	}



	// Setters:
	void Renderer::SetActiveCamera(const Float3& position, const Float4x4& viewMatrix, const Float4x4& projectionMatrix)
	{
		s_pIRenderer->SetActiveCamera(position, viewMatrix, projectionMatrix);
	}
	void Renderer::SetDepthBiasConstantFactor(float depthBiasConstantFactor)
	{
		s_pIRenderer->SetDepthBiasConstantFactor(depthBiasConstantFactor);
	}
	void Renderer::SetDepthBiasClamp(float depthBiasClamp)
	{
		s_pIRenderer->SetDepthBiasClamp(depthBiasClamp);
	}
	void Renderer::SetDepthBiasSlopeFactor(float depthBiasSlopeFactor)
	{
		s_pIRenderer->SetDepthBiasSlopeFactor(depthBiasSlopeFactor);
	}
	void Renderer::SetOutlineColor(const Float4& outlineColor)
	{
		s_pIRenderer->SetOutlineColor(outlineColor);
	}
	void Renderer::SetOutlineThickness(int outlineThickness)
	{
		s_pIRenderer->SetOutlineThickness(outlineThickness);
	}



	// Functionallity forwarding:
	void Renderer::CollectGarbage()
	{
		s_pIRenderer->CollectGarbage();
	}
	void Renderer::WaitDeviceIdle()
	{
		s_pIRenderer->WaitDeviceIdle();
	}
	void Renderer::WaitForFrameFinished(uint32_t frameIndex)
	{
        s_pIRenderer->WaitForFrameFinished(frameIndex);
	}



    // Debugging:
	void Renderer::DumpVmaBufferAllocations()
    {
        s_pIRenderer->DumpVmaBufferAllocations();
    }
	void Renderer::DumpVmaImageAllocations()
    {
        s_pIRenderer->DumpVmaImageAllocations();
    }



	// Gpu resource factories:
	emberBackendInterface::IComputeShader* Renderer::CreateComputeShader(const std::filesystem::path& computeSpv, const std::string& name)
	{
		return s_pIRenderer->CreateComputeShader(computeSpv, name);
	}
	emberBackendInterface::IBuffer* Renderer::CreateBuffer(uint32_t count, uint32_t elementSize, emberCommon::BufferUsage usage)
	{
		return s_pIRenderer->CreateBuffer(count, elementSize, usage);
	}
	//static emberBackendInterface::ITexture* Renderer::CreateTexture1d(const std::string& name, int width, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	//{
	//
	//}
	emberBackendInterface::ITexture* Renderer::CreateTexture2d(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		return s_pIRenderer->CreateTexture2d(width, height, format, usage, data);
	}
	emberBackendInterface::ITexture* Renderer::CreateTexture3d(int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		return s_pIRenderer->CreateTexture3d(width, height, depth, format, usage, data);
	}
	emberBackendInterface::ITexture* Renderer::CreateTextureCube(int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		return s_pIRenderer->CreateTextureCube(width, height, format, usage, data);
	}
	emberBackendInterface::IMesh* Renderer::CreateMesh(const std::string& name)
	{
		return s_pIRenderer->CreateMesh();
	}
	emberBackendInterface::IDescriptorSetBinding* Renderer::CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pIMaterial)
	{
		return s_pIRenderer->CreateDrawCallDescriptorSetBinding(pIMaterial);
	}
	

	
	// Gpu resource destruction:
	void Renderer::DestroyComputeShader(emberBackendInterface::IComputeShader* pIComputeShader)
	{
		if (!pIComputeShader)
			return;
		s_pIRenderer->DestroyComputeShader(pIComputeShader);
	}
}