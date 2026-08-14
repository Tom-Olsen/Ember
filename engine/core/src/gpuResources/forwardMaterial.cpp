#include "forwardMaterial.h"
#include "iMaterial.h"
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



	// Cloning:
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
			throw std::runtime_error("ForwardMaterial::GetRenderMode() failed. Material is invalid or expired.");
		return pIMaterial->GetForwardRenderMode();
	}
	const emberCommon::ForwardRenderState& ForwardMaterial::GetRenderState() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("ForwardMaterial::GetRenderState() failed. Material is invalid or expired.");
		return pIMaterial->GetForwardRenderState();
	}
	ShadowMaterial ForwardMaterial::GetShadowMaterial() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			return ShadowMaterial();

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
			throw std::runtime_error("ForwardMaterial::GetRenderQueue() failed. Material is invalid or expired.");
		return pIMaterial->GetRenderQueue();
	}
	bool ForwardMaterial::GetIsTransparent() const
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("ForwardMaterial::GetIsTransparent() failed. Material is invalid or expired.");
		return pIMaterial->IsTransparent();
	}



	// Setters:
	void ForwardMaterial::SetRenderMode(emberCommon::ForwardRenderMode renderMode)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("ForwardMaterial::SetRenderMode(...) failed. Material is invalid or expired.");
		pIMaterial->SetForwardRenderMode(renderMode);
	}
	void ForwardMaterial::SetCullMode(emberCommon::CullMode cullMode)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("ForwardMaterial::SetCullMode(...) failed. Material is invalid or expired.");
		pIMaterial->SetCullMode(cullMode);
	}
	void ForwardMaterial::SetRenderQueue(int32_t renderQueue)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("ForwardMaterial::SetRenderQueue(...) failed. Material is invalid or expired.");
		pIMaterial->SetRenderQueue(renderQueue);
	}
	void ForwardMaterial::SetShadowMaterial(const ShadowMaterial& shadowMaterial)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("ForwardMaterial::SetShadowMaterial(...) failed. Material is invalid or expired.");

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