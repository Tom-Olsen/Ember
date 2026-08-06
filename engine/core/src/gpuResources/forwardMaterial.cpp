#include "forwardMaterial.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialManager.h"
#include <stdexcept>



namespace emberCore
{
	// Private methods:
	ForwardMaterial::ForwardMaterial(emberBackendInterface::IMaterial* pIMaterial)
		: Material(pIMaterial)
	{
		if (pIMaterial->GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("ForwardMaterial::ForwardMaterial(...) failed. IMaterial is not a forward material.");
	}



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
		return MaterialManager::CloneForwardMaterial(name, *this);
	}
	ForwardMaterial ForwardMaterial::Clone(const std::string& name, emberCommon::ForwardRenderMode renderMode) const
	{
		return MaterialManager::CloneForwardMaterial(name, *this, renderMode);
	}



	// Getters:
	emberCommon::ForwardRenderMode ForwardMaterial::GetRenderMode() const
	{
		return m_pIMaterial->GetForwardRenderMode();
	}
	const emberCommon::ForwardRenderState& ForwardMaterial::GetRenderState() const
	{
		return m_pIMaterial->GetForwardRenderState();
	}
	ShadowMaterial ForwardMaterial::GetShadowMaterial() const
	{
		emberBackendInterface::IMaterial* pIShadowMaterial = m_pIMaterial->GetShadowMaterial();
        if (pIShadowMaterial == nullptr)
			throw std::runtime_error(GetName() + "has no shadow material not found.");
		return ShadowMaterial{ pIShadowMaterial };
	}
	ShadowMaterial ForwardMaterial::TryGetShadowMaterial() const
	{
		emberBackendInterface::IMaterial* pIShadowMaterial = m_pIMaterial->GetShadowMaterial();
        if (pIShadowMaterial == nullptr)
        {
			LOG_ERROR("Material '{}' has no shadow material!", GetName());
            return ShadowMaterial();
		}
		return ShadowMaterial{ pIShadowMaterial };
	}
	int32_t ForwardMaterial::GetRenderQueue() const
	{
		return m_pIMaterial->GetRenderQueue();
	}
	bool ForwardMaterial::GetIsTransparent() const
	{
		return m_pIMaterial->IsTransparent();
	}



	// Setters:
	void ForwardMaterial::SetRenderMode(emberCommon::ForwardRenderMode renderMode)
	{
		m_pIMaterial->SetForwardRenderMode(renderMode);
	}
	void ForwardMaterial::SetCullMode(emberCommon::CullMode cullMode)
	{
		m_pIMaterial->SetCullMode(cullMode);
	}
	void ForwardMaterial::SetRenderQueue(int32_t renderQueue)
	{
		m_pIMaterial->SetRenderQueue(renderQueue);
	}
	void ForwardMaterial::SetShadowMaterial(const ShadowMaterial& shadowMaterial)
	{
		m_pIMaterial->SetShadowMaterial(shadowMaterial.GetInterfaceHandle());
	}
}