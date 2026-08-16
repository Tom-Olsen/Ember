#include "forwardMaterial.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialManager.h"
#include <stdexcept>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	ForwardMaterial::ForwardMaterial()
		: Material()
	{

	}
	ForwardMaterial::~ForwardMaterial()
	{

	}



	// Creation/Cloning:
	ForwardMaterial ForwardMaterial::CreateFromShader(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& name)
	{
		return MaterialManager::CreateForwardMaterial(renderMode, materialShader, name);
	}
	ForwardMaterial ForwardMaterial::Clone(const std::string& name) const
	{
		return MaterialManager::CloneForwardMaterial(*this, name);
	}
	ForwardMaterial ForwardMaterial::Clone(const std::string& name, emberCommon::ForwardRenderMode renderMode) const
	{
		return MaterialManager::CloneForwardMaterial(*this, renderMode, name);
	}



	// Getters:
	emberCommon::ForwardRenderMode ForwardMaterial::GetRenderMode() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("ForwardMaterial::GetRenderMode() failed. Material is invalid or expired.");
			return emberCommon::ForwardRenderMode::count;
		}
		return pIMaterial->GetForwardRenderMode();
	}
	const emberCommon::ForwardRenderState* ForwardMaterial::GetRenderState() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("ForwardMaterial::GetRenderState() failed. Material is invalid or expired.");
			return nullptr;
		}
		return &pIMaterial->GetForwardRenderState();
	}
	ShadowMaterial ForwardMaterial::GetShadowMaterial() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("ForwardMaterial::GetShadowMaterial() failed. Material is invalid or expired.");
			return ShadowMaterial();
		}

		emberBackendInterface::IMaterial* pIShadowMaterial = pIMaterial->GetShadowMaterial();
		if (pIShadowMaterial == nullptr)
			return ShadowMaterial();

		MaterialId shadowMaterialId = MaterialManager::GetMaterialId(pIShadowMaterial);
		if (shadowMaterialId.index == invalidMaterialId.index)
			return ShadowMaterial();
		return ShadowMaterial{ shadowMaterialId };
	}
	int32_t ForwardMaterial::GetRenderQueue() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("ForwardMaterial::GetRenderQueue() failed. Material is invalid or expired.");
			return 0;
		}
		return pIMaterial->GetRenderQueue();
	}
	bool ForwardMaterial::GetIsTransparent() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("ForwardMaterial::GetIsTransparent() failed. Material is invalid or expired.");
			return false;
		}
		return pIMaterial->IsTransparent();
	}



	// Setters:
	void ForwardMaterial::SetRenderMode(emberCommon::ForwardRenderMode renderMode)
	{
		if (emberBackendInterface::IMaterial* pIMaterial = GetMutableInterfaceHandle())
			pIMaterial->SetForwardRenderMode(renderMode);
		else
			LOG_WARN("ForwardMaterial::SetRenderMode(...) failed. Material is invalid, expired, or immutable.");
	}
	void ForwardMaterial::SetCullMode(emberCommon::CullMode cullMode)
	{
		if (emberBackendInterface::IMaterial* pIMaterial = GetMutableInterfaceHandle())
			pIMaterial->SetCullMode(cullMode);
		else
			LOG_WARN("ForwardMaterial::SetCullMode(...) failed. Material is invalid, expired, or immutable.");
	}
	void ForwardMaterial::SetRenderQueue(int32_t renderQueue)
	{
		if (emberBackendInterface::IMaterial* pIMaterial = GetMutableInterfaceHandle())
			pIMaterial->SetRenderQueue(renderQueue);
		else
			LOG_WARN("ForwardMaterial::SetRenderQueue(...) failed. Material is invalid, expired, or immutable.");
	}
	void ForwardMaterial::SetShadowMaterial(const ShadowMaterial& shadowMaterial)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetMutableInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("ForwardMaterial::SetShadowMaterial(...) failed. Material is invalid, expired, or immutable.");
			return;
		}

		emberBackendInterface::IMaterial* pShadowMaterial = shadowMaterial.GetInterfaceHandle();
		if (pShadowMaterial == nullptr)
			throw std::runtime_error("ForwardMaterial::SetShadowMaterial(...) failed. Shadow material is invalid or expired.");

		pIMaterial->SetShadowMaterial(pShadowMaterial);
	}



	// Private methods:
	ForwardMaterial::ForwardMaterial(MaterialId materialId)
		: Material(materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("ForwardMaterial::ForwardMaterial(...) failed. Material is invalid or expired.");
		if (pIMaterial->GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("ForwardMaterial::ForwardMaterial(...) failed. IMaterial is not a forward material.");
	}
}