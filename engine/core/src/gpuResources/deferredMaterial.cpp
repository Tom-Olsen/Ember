#include "deferredMaterial.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialManager.h"
#include <stdexcept>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	DeferredMaterial::DeferredMaterial()
		: Material()
	{

	}
	DeferredMaterial::~DeferredMaterial()
	{

	}



	// Creation/Cloning:
	DeferredMaterial DeferredMaterial::CreateFromShader(const MaterialShader& materialShader, const std::string& name)
	{
		return MaterialManager::CreateDeferredGeometryMaterial(materialShader, name);
	}
	DeferredMaterial DeferredMaterial::Clone(const std::string& name) const
	{
		return MaterialManager::CloneDeferredGeometryMaterial(*this, name);
	}
	DeferredMaterial DeferredMaterial::CloneWithDefaultBindings(const std::string& name) const
	{
		return MaterialManager::CloneDeferredGeometryMaterialWithDefaultBindings(*this, name);
	}



	// Getters:
	const emberCommon::DeferredGeometryRenderState* DeferredMaterial::GetRenderState() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("DeferredMaterial::GetRenderState() failed. Material is invalid or expired.");
			return nullptr;
		}
		return &pIMaterial->GetDeferredGeometryRenderState();
	}
	ShadowMaterial DeferredMaterial::GetShadowMaterial() const
	{
		if (GetInterfaceHandle() == nullptr)
		{
			LOG_WARN("DeferredMaterial::GetShadowMaterial() failed. Material is invalid or expired.");
			return ShadowMaterial();
		}

		emberCommon::MaterialId shadowMaterialId = MaterialManager::TryGetShadowMaterialIdOfSurfaceMaterial(m_materialId);
		if (shadowMaterialId.index == emberCommon::invalidMaterialId.index)
			return ShadowMaterial();
		return ShadowMaterial{ shadowMaterialId };
	}
	int32_t DeferredMaterial::GetRenderQueue() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("DeferredMaterial::GetRenderQueue() failed. Material is invalid or expired.");
			return 0;
		}
		return pIMaterial->GetRenderQueue();
	}



	// Setters:
	void DeferredMaterial::SetCullMode(emberCommon::CullMode cullMode)
	{
		if (emberBackendInterface::IMaterial* pIMaterial = GetMutableInterfaceHandle())
			pIMaterial->SetCullMode(cullMode);
		else
			LOG_WARN("DeferredMaterial::SetCullMode(...) failed. Material is invalid, expired, or immutable.");
	}
	void DeferredMaterial::SetRenderQueue(int32_t renderQueue)
	{
		if (emberBackendInterface::IMaterial* pIMaterial = GetMutableInterfaceHandle())
			pIMaterial->SetRenderQueue(renderQueue);
		else
			LOG_WARN("DeferredMaterial::SetRenderQueue(...) failed. Material is invalid, expired, or immutable.");
	}
	void DeferredMaterial::SetShadowMaterial(const ShadowMaterial& shadowMaterial)
	{
		if (GetMutableInterfaceHandle() == nullptr)
		{
			LOG_WARN("DeferredMaterial::SetShadowMaterial(...) failed. Material is invalid, expired, or immutable.");
			return;
		}
		MaterialManager::SetShadowMaterial(m_materialId, shadowMaterial.m_materialId);
	}
	void DeferredMaterial::ResetShadowMaterial()
	{
		if (GetMutableInterfaceHandle() == nullptr)
		{
			LOG_WARN("DeferredMaterial::ResetShadowMaterial() failed. Material is invalid, expired, or immutable.");
			return;
		}
		MaterialManager::ResetShadowMaterial(m_materialId);
	}



	// Private methods:
	DeferredMaterial::DeferredMaterial(emberCommon::MaterialId materialId)
		: Material(materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("DeferredMaterial::DeferredMaterial(...) failed. Material is invalid or expired.");
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
			throw std::runtime_error("DeferredMaterial::DeferredMaterial(...) failed. IMaterial is not a deferred material.");
	}
}