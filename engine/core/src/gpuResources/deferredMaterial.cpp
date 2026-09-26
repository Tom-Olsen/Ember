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



	// Cloning:
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
		emberBackendInterface::IMaterial* pIMaterial = TryGetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("DeferredMaterial::GetRenderState() failed. Material is invalid or expired.");
			return nullptr;
		}
		return &pIMaterial->GetDeferredGeometryRenderState();
	}
	int32_t DeferredMaterial::GetRenderQueue() const
	{
		emberBackendInterface::IMaterial* pIMaterial = TryGetInterfaceHandle();
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
		if (emberBackendInterface::IMaterial* pIMaterial = TryGetMutableInterfaceHandle())
			pIMaterial->SetCullMode(cullMode);
		else
			LOG_WARN("DeferredMaterial::SetCullMode(...) failed. Material is invalid, expired, or immutable.");
	}
	void DeferredMaterial::SetRenderQueue(int32_t renderQueue)
	{
		if (emberBackendInterface::IMaterial* pIMaterial = TryGetMutableInterfaceHandle())
			pIMaterial->SetRenderQueue(renderQueue);
		else
			LOG_WARN("DeferredMaterial::SetRenderQueue(...) failed. Material is invalid, expired, or immutable.");
	}
	void DeferredMaterial::SetShadowMaterial(const ShadowMaterial& shadowMaterial)
	{
		if (TryGetMutableInterfaceHandle() == nullptr)
		{
			LOG_WARN("DeferredMaterial::SetShadowMaterial(...) failed. Material is invalid, expired, or immutable.");
			return;
		}
		MaterialManager::SetShadowMaterial(m_materialId, shadowMaterial.m_materialId);
	}
	void DeferredMaterial::ResetShadowMaterial()
	{
		if (TryGetMutableInterfaceHandle() == nullptr)
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
		emberBackendInterface::IMaterial* pIMaterial = TryGetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("DeferredMaterial::DeferredMaterial(...) failed. Material is invalid or expired.");
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
			throw std::runtime_error("DeferredMaterial::DeferredMaterial(...) failed. IMaterial is not a deferred material.");
	}
}