#include "renderer.h"
#include "buffer.h"
#include "emberTime.h"
#include "iMaterial.h"
#include "iRenderer.h"
#include "mesh.h"
#include "material.h"
#include "shaderProperties.h"
#include "texture.h"
#include "texture2d.h"
#include "window.h"
#include "logger.h"



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
		s_isInitialized = true;

		s_pointLightRotationMatrices[0] = Float4x4::identity;
		s_pointLightRotationMatrices[1] = Float4x4::RotateY( math::pi2);
		s_pointLightRotationMatrices[2] = Float4x4::RotateY( math::pi );
		s_pointLightRotationMatrices[3] = Float4x4::RotateY(-math::pi2);
		s_pointLightRotationMatrices[4] = Float4x4::RotateX( math::pi2);
		s_pointLightRotationMatrices[5] = Float4x4::RotateX(-math::pi2);

		s_pIRenderer = std::unique_ptr<emberBackendInterface::IRenderer>(pIRenderer);
	}
	void Renderer::Clear()
	{
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
    void Renderer::DrawOutline(const Mesh& mesh, const Float4x4& localToWorldMatrix)
    {
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
        s_pIRenderer->DrawOutline(pIMesh, localToWorldMatrix, 0);
    }
	void Renderer::DrawMesh(const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows, emberCommon::CullMode cullMode)
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
		s_pIRenderer->DrawMesh(pIMesh, pIMaterial, pICallDescriptorSetBinding, localToWorldMatrix, receiveShadows, castShadows, cullMode, 0);
	}
	ShaderProperties Renderer::DrawMesh(const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows, emberCommon::CullMode cullMode)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawMesh(pIMesh, pIMaterial, localToWorldMatrix, receiveShadows, castShadows, cullMode, 0);
		return ShaderProperties(pICallDescriptorSetBinding);
	}
	void Renderer::DrawGizmo(const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, emberCommon::CullMode cullMode)
	{
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = shaderProperties.GetCallInterfaceHandle();
		if (!pICallDescriptorSetBinding)
		{
			LOG_WARN("Renderer::DrawGizmo(...) skipped stale ShaderProperties. Reassign ShaderProperties before reusing it for another draw call.");
			return;
		}
		s_pIRenderer->DrawGizmo(pIMesh, pIMaterial, pICallDescriptorSetBinding, localToWorldMatrix, cullMode, 0);
	}
	ShaderProperties Renderer::DrawGizmo(const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, emberCommon::CullMode cullMode)
	{
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawGizmo(pIMesh, pIMaterial, localToWorldMatrix, cullMode, 0);
		return ShaderProperties(pICallDescriptorSetBinding);
	}



	// Draw instanced:
    void Renderer::DrawOutlineInstanced(const Mesh& mesh, const Float4x4& localToWorldMatrix, uint32_t instanceCount)
    {
        LOG_WARN("Renderer::DrawOutlineInstanced not supported yet!");
		//emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
        //s_pIRenderer->DrawOutline(pIMesh, localToWorldMatrix, instanceCount);
    }
	void Renderer::DrawMeshInstanced(uint32_t instanceCount, Buffer& instanceBuffer, const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows, emberCommon::CullMode cullMode)
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
		s_pIRenderer->DrawMesh(pIMesh, pIMaterial, pICallDescriptorSetBinding, localToWorldMatrix, receiveShadows, castShadows, cullMode, instanceCount);
	}
	ShaderProperties Renderer::DrawMeshInstanced(uint32_t instanceCount, Buffer& instanceBuffer, const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows, emberCommon::CullMode cullMode)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawMesh(pIMesh, pIMaterial, localToWorldMatrix, receiveShadows, castShadows, cullMode, instanceCount);
		ShaderProperties shaderProperties(pICallDescriptorSetBinding);
		if (shaderProperties.HasBinding("instanceBuffer"))
			shaderProperties.SetBuffer("instanceBuffer", instanceBuffer);
		return shaderProperties;
	}
	void Renderer::DrawMeshInstanced(uint32_t instanceCount, const Mesh& mesh, const Material& material, ShaderProperties& shaderProperties, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows, emberCommon::CullMode cullMode)
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
		s_pIRenderer->DrawMesh(pIMesh, pIMaterial, pICallDescriptorSetBinding, localToWorldMatrix, receiveShadows, castShadows, cullMode, instanceCount);
	}
	ShaderProperties Renderer::DrawMeshInstanced(uint32_t instanceCount, const Mesh& mesh, const Material& material, const Float4x4& localToWorldMatrix, bool receiveShadows, bool castShadows, emberCommon::CullMode cullMode)
	{
		if (material.GetName() == "errorMaterial")
			receiveShadows = castShadows = false;
		emberBackendInterface::IMesh* pIMesh = mesh.GetInterfaceHandle();
		emberBackendInterface::IMaterial* pIMaterial = material.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawMesh(pIMesh, pIMaterial, localToWorldMatrix, receiveShadows, castShadows, cullMode, instanceCount);
		return ShaderProperties(pICallDescriptorSetBinding);
	}



	// Getters:
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



    // Private Methods:
	// Gpu resource factories:
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
	emberBackendInterface::IComputeShader* Renderer::CreateComputeShader(const std::string& name, const std::filesystem::path& computeSpv)
	{
		return s_pIRenderer->CreateComputeShader(name, computeSpv);
	}
	emberBackendInterface::IMaterial* Renderer::CreateForwardMaterial(emberCommon::RenderMode renderMode, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		return s_pIRenderer->CreateForwardMaterial(name, renderMode, renderQueue, vertexSpv, fragmentSpv);
	}
	emberBackendInterface::IMaterial* Renderer::CreateShadowMaterial(const std::string& name, const std::filesystem::path& vertexSpv)
	{
		return s_pIRenderer->CreateShadowMaterial(name, vertexSpv);
	}
	emberBackendInterface::IMesh* Renderer::CreateMesh(const std::string& name)
	{
		return s_pIRenderer->CreateMesh();
	}
	emberBackendInterface::IDescriptorSetBinding* Renderer::CreateDrawCallDescriptorSetBinding(emberBackendInterface::IMaterial* pIMaterial)
	{
		return s_pIRenderer->CreateDrawCallDescriptorSetBinding(pIMaterial);
	}
}