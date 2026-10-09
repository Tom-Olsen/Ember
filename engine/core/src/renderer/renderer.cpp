#include "renderer.h"
#include "buffer.h"
#include "callProperties.h"
#include "commonMaterialPass.h"
#include "emberTime.h"
#include "gpuResourceFactory.h"
#include "iMaterial.h"
#include "iRenderer.h"
#include "logger.h"
#include "material.h"
#include "materialManager.h"
#include "mesh.h"
#include "shadowMaterial.h"
#include "texture.h"
#include "texture2d.h"
#include "textureManager.h"
#include "window.h"
#include <cassert>
#include <stdexcept>



namespace emberCore
{
	// Static members:
	bool Renderer::s_isInitialized = false;
	emberBackendInterface::IRenderer* Renderer::s_pIRenderer = nullptr;
	std::array<Float4x4, 6> Renderer::s_pointLightRotationMatrices;
	emberBackendInterface::IRenderer* Renderer::GetInterfaceHandle()
	{
		return s_pIRenderer;
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
		s_pointLightRotationMatrices[1] = Float4x4::RotateY(Radians(math::pi2));
		s_pointLightRotationMatrices[2] = Float4x4::RotateY(Radians(math::pi));
		s_pointLightRotationMatrices[3] = Float4x4::RotateY(Radians(-math::pi2));
		s_pointLightRotationMatrices[4] = Float4x4::RotateX(Radians(math::pi2));
		s_pointLightRotationMatrices[5] = Float4x4::RotateX(Radians(-math::pi2));

		s_pIRenderer = pIRenderer;
		s_isInitialized = true;
	}
	void Renderer::Clear()
	{
		s_pIRenderer = nullptr;
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

		// Missing mesh:
		if (pIMesh == nullptr)
		{
			LOG_WARN("Renderer::DrawOutline(...) received an invalid or moved-from mesh.");
			return;
		}

		// Valid draw call:
		CallProperties callProperties(s_pIRenderer->DrawOutline(pIMesh, 0));
		SetModelData(localToWorldMatrix, callProperties);
    }
	CallProperties Renderer::DrawMesh(const DrawData& drawData)
	{
		ValidateDrawData(drawData);
		emberBackendInterface::IMaterial* pIMaterial = ResolveSurfaceMaterial(drawData.material);
		emberCommon::CullMode cullMode = ResolveCullMode(drawData, pIMaterial);
		emberBackendInterface::IMesh* pIMesh = drawData.mesh.GetInterfaceHandle();

		// Missing mesh:
		if (pIMesh == nullptr)
		{
			LOG_WARN("Renderer::DrawMesh(...) received an invalid or moved-from mesh.");
			return CallProperties();
		}

		// Error material fallback:
		bool receiveShadows = drawData.receiveShadows;
		bool castShadows = drawData.castShadows;
		if (pIMaterial == MaterialManager::s_pIErrorMaterial)
			receiveShadows = castShadows = false;

		// Valid draw call:
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawMesh(drawData.localToWorldMatrix.GetTranslation(), pIMesh, pIMaterial, cullMode, receiveShadows, drawData.instanceCount);
		CallProperties callProperties(pICallDescriptorSetBinding);
		SetModelData(drawData.localToWorldMatrix, callProperties);
		SetInstanceBuffer(drawData, callProperties);
		if (castShadows)
			callProperties.SetShadowProperties(DrawMeshShadow(drawData));
		return callProperties;
	}
	void Renderer::DrawMesh(const DrawData& drawData, CallProperties& callProperties)
	{
		ValidateDrawData(drawData);
		emberBackendInterface::IMaterial* pIMaterial = ResolveSurfaceMaterial(drawData.material);
		emberCommon::CullMode cullMode = ResolveCullMode(drawData, pIMaterial);
		emberBackendInterface::IMesh* pIMesh = drawData.mesh.GetInterfaceHandle();

		// Missing mesh:
		if (pIMesh == nullptr)
		{
			LOG_WARN("Renderer::DrawMesh(...) received an invalid or moved-from mesh.");
			return;
		}

		// Error material fallback:
		if (pIMaterial == MaterialManager::s_pIErrorMaterial)
		{
			CallProperties fallbackCallProperties(s_pIRenderer->DrawMesh(drawData.localToWorldMatrix.GetTranslation(), pIMesh, pIMaterial, cullMode, false, drawData.instanceCount));
			SetModelData(drawData.localToWorldMatrix, fallbackCallProperties);
			SetInstanceBuffer(drawData, fallbackCallProperties);
			return;
		}

		// Invalid callProperties fallback:
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = callProperties.GetCallInterfaceHandle();
		if (pICallDescriptorSetBinding == nullptr)
		{
			LOG_WARN("Renderer::DrawMesh(...) received invalid CallProperties. Drawing with default call properties. Reassign CallProperties before reusing it for another draw call.");
			CallProperties fallbackCallProperties(s_pIRenderer->DrawMesh(drawData.localToWorldMatrix.GetTranslation(), pIMesh, pIMaterial, cullMode, drawData.receiveShadows, drawData.instanceCount));
			SetModelData(drawData.localToWorldMatrix, fallbackCallProperties);
			SetInstanceBuffer(drawData, fallbackCallProperties);
			if (drawData.castShadows)
				fallbackCallProperties.SetShadowProperties(DrawMeshShadow(drawData));
			return;
		}

		// Valid draw call:
		SetModelData(drawData.localToWorldMatrix, callProperties);
		SetInstanceBuffer(drawData, callProperties);
		s_pIRenderer->DrawMesh(drawData.localToWorldMatrix.GetTranslation(), pIMesh, pIMaterial, pICallDescriptorSetBinding, cullMode, drawData.receiveShadows, drawData.instanceCount);
		if (drawData.castShadows)
		{
			if (callProperties.HasShadowProperties())
				DrawMeshShadow(drawData, callProperties.GetShadowProperties());
			else
				callProperties.SetShadowProperties(DrawMeshShadow(drawData));
		}
	}
	CallProperties Renderer::DrawGizmo(const DrawData& drawData)
	{
		ValidateDrawData(drawData);
		emberBackendInterface::IMaterial* pIMaterial = ResolveGizmoMaterial(drawData.material);
		emberCommon::CullMode cullMode = ResolveCullMode(drawData, pIMaterial);
		emberBackendInterface::IMesh* pIMesh = drawData.mesh.GetInterfaceHandle();

		// Missing mesh:
		if (pIMesh == nullptr)
		{
			LOG_WARN("Renderer::DrawGizmo(...) received an invalid or moved-from mesh.");
			return CallProperties();
		}

		// Valid draw call:
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawGizmo(drawData.localToWorldMatrix.GetTranslation(), pIMesh, pIMaterial, cullMode, drawData.instanceCount);
		CallProperties callProperties(pICallDescriptorSetBinding);
		SetModelData(drawData.localToWorldMatrix, callProperties);
		SetInstanceBuffer(drawData, callProperties);
		return callProperties;
	}
	void Renderer::DrawGizmo(const DrawData& drawData, CallProperties& callProperties)
	{
		ValidateDrawData(drawData);
		emberBackendInterface::IMaterial* pIMaterial = ResolveGizmoMaterial(drawData.material);
		emberCommon::CullMode cullMode = ResolveCullMode(drawData, pIMaterial);
		emberBackendInterface::IMesh* pIMesh = drawData.mesh.GetInterfaceHandle();

		// Missing mesh:
		if (pIMesh == nullptr)
		{
			LOG_WARN("Renderer::DrawGizmo(...) received an invalid or moved-from mesh.");
			return;
		}

		// Error gizmo material fallback:
		if (pIMaterial == MaterialManager::s_pIErrorGizmoMaterial)
		{
			CallProperties fallbackCallProperties(s_pIRenderer->DrawGizmo(drawData.localToWorldMatrix.GetTranslation(), pIMesh, pIMaterial, cullMode, drawData.instanceCount));
			SetModelData(drawData.localToWorldMatrix, fallbackCallProperties);
			SetInstanceBuffer(drawData, fallbackCallProperties);
			return;
		}

		// Invalid callProperties fallback:
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = callProperties.GetCallInterfaceHandle();
		if (pICallDescriptorSetBinding == nullptr)
		{
			LOG_WARN("Renderer::DrawGizmo(...) received invalid CallProperties. Drawing with default call properties. Reassign CallProperties before reusing it for another draw call.");
			CallProperties fallbackCallProperties(s_pIRenderer->DrawGizmo(drawData.localToWorldMatrix.GetTranslation(), pIMesh, pIMaterial, cullMode, drawData.instanceCount));
			SetModelData(drawData.localToWorldMatrix, fallbackCallProperties);
			SetInstanceBuffer(drawData, fallbackCallProperties);
			return;
		}

		// Valid draw call:
		SetModelData(drawData.localToWorldMatrix, callProperties);
		SetInstanceBuffer(drawData, callProperties);
		s_pIRenderer->DrawGizmo(drawData.localToWorldMatrix.GetTranslation(), pIMesh, pIMaterial, pICallDescriptorSetBinding, cullMode, drawData.instanceCount);
	}



	// Getters:
	uint32_t Renderer::GetRenderWidth()
	{
		return s_pIRenderer->GetRenderWidth();
	}
	uint32_t Renderer::GetRenderHeight()
	{
		return s_pIRenderer->GetRenderHeight();
	}
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
	Texture2d Renderer::GetFinalRenderTexture()
	{
		return TextureManager::GetTexture<Texture2d>(TextureManager::s_finalRenderTextureName);
	}
	Texture2d Renderer::GetGizmoTexture()
	{
		return TextureManager::GetTexture<Texture2d>(TextureManager::s_gizmoTextureName);
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
	void Renderer::DumpBufferAllocations()
    {
        s_pIRenderer->DumpBufferAllocations();
    }
	void Renderer::DumpImageAllocations()
    {
        s_pIRenderer->DumpImageAllocations();
    }



	// Private methods:
	// Draw mesh helpers:
	void Renderer::ValidateDrawData(const DrawData& drawData)
	{
		if (drawData.instanceCount == 0 && drawData.pInstanceBuffer != nullptr)
			LOG_WARN("Renderer received an instance buffer with instanceCount 0. Ignoring the instance buffer for this non-instanced draw.");
		if (drawData.instanceCount > 0 && drawData.pInstanceBuffer != nullptr && drawData.instanceCount > drawData.pInstanceBuffer->GetCount())
			LOG_WARN("Renderer received an instanceCount that exceeds the instance buffer element count.");
	}
	emberCommon::CullMode Renderer::ResolveCullMode(const DrawData& drawData, emberBackendInterface::IMaterial* pIMaterial)
	{
		assert(pIMaterial != nullptr);
		assert(drawData.cullMode != emberCommon::CullMode::count);
		emberCommon::CullMode cullMode = (drawData.cullMode == emberCommon::CullMode::materialDefault) ? pIMaterial->GetCullMode() : drawData.cullMode;
		assert(cullMode != emberCommon::CullMode::count);
		assert(cullMode != emberCommon::CullMode::materialDefault);
		return cullMode;
	}



	// Set default bindings:
	void Renderer::SetModelData(const Float4x4& localToWorldMatrix, CallProperties& callProperties)
	{
		if (!callProperties.HasBinding("ModelMatrizes"))
			return;

		callProperties.SetValue("ModelMatrizes", "model_localToWorldMatrix", localToWorldMatrix);
		callProperties.SetValue("ModelMatrizes", "model_worldToLocalMatrix", localToWorldMatrix.Inverse());
	}
	void Renderer::SetInstanceBuffer(const DrawData& drawData, CallProperties& callProperties)
	{
		if (drawData.instanceCount == 0 || drawData.pInstanceBuffer == nullptr)
			return;
		if (!callProperties.HasBinding("instanceBuffer"))
		{
			LOG_WARN("Renderer received an instance buffer, but the draw shader does not expose an instanceBuffer binding. Drawing without the instance buffer.");
			return;
		}
		callProperties.SetBuffer("instanceBuffer", *drawData.pInstanceBuffer);
	}



	// Shadow draws calls:
	CallProperties Renderer::DrawMeshShadow(const DrawData& drawData)
	{
		ShadowMaterial shadowMaterial = drawData.material.GetShadowMaterial();
		emberBackendInterface::IMaterial* pIMaterial = shadowMaterial.TryGetInterfaceHandle();
		if (pIMaterial == nullptr)
			return CallProperties();

		// Valid draw call:
		emberBackendInterface::IMesh* pIMesh = drawData.mesh.GetInterfaceHandle();
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = s_pIRenderer->DrawMeshShadow(pIMesh, pIMaterial, drawData.instanceCount);
		CallProperties callProperties(pICallDescriptorSetBinding);
		SetModelData(drawData.localToWorldMatrix, callProperties);
		SetInstanceBuffer(drawData, callProperties);
		return callProperties;
	}
	void Renderer::DrawMeshShadow(const DrawData& drawData, CallProperties& callProperties)
	{
		ShadowMaterial shadowMaterial = drawData.material.GetShadowMaterial();
		emberBackendInterface::IMaterial* pIMaterial = shadowMaterial.TryGetInterfaceHandle();
		if (pIMaterial == nullptr)
			return;

		// Invalid callProperties fallback:
		emberBackendInterface::IDescriptorSetBinding* pICallDescriptorSetBinding = callProperties.GetCallInterfaceHandle();
		if (pICallDescriptorSetBinding == nullptr)
		{
			LOG_WARN("Renderer::DrawMesh(...) received invalid shadow CallProperties. Drawing the shadow with default call properties.");
			CallProperties fallbackCallProperties = DrawMeshShadow(drawData);
			return;
		}

		// Valid draw call:
		SetModelData(drawData.localToWorldMatrix, callProperties);
		SetInstanceBuffer(drawData, callProperties);
		emberBackendInterface::IMesh* pIMesh = drawData.mesh.GetInterfaceHandle();
		s_pIRenderer->DrawMeshShadow(pIMesh, pIMaterial, pICallDescriptorSetBinding, drawData.instanceCount);
	}



	// Material resolution:
	emberBackendInterface::IMaterial* Renderer::ResolveSurfaceMaterial(const Material& material)
	{
		// Resolve invalid and non-surface materials to the error material:
		if (emberBackendInterface::IMaterial* pIMaterial = material.TryGetInterfaceHandle())
		{
			if (emberCommon::IsSurfaceMaterialPass(pIMaterial->GetMaterialPass()))
				return pIMaterial;
		}
		return MaterialManager::s_pIErrorMaterial;
	}
	emberBackendInterface::IMaterial* Renderer::ResolveGizmoMaterial(const Material& material)
	{
		// Resolve invalid and non-gizmo materials to the error gizmo material:
		if (emberBackendInterface::IMaterial* pIMaterial = material.TryGetInterfaceHandle())
		{
			if (pIMaterial->GetMaterialPass() == emberCommon::MaterialPass::gizmo)
				return pIMaterial;
		}
		return MaterialManager::s_pIErrorGizmoMaterial;
	}
}