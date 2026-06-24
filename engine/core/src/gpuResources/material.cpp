#include "material.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialManager.h"



namespace emberCore
{
	// Private methods:
	emberBackendInterface::IMaterial* const Material::GetInterfaceHandle() const
	{
		return m_pIMaterial;
	}
	Material::Material(emberBackendInterface::IMaterial* pIMaterial)
        : Shader(pIMaterial->GetShaderDescriptorSetBinding())
	{
		m_pIMaterial = pIMaterial;
	}



	// Public methods:
	// Constructor/Destructor:
	Material::Material()
	{
		m_pIMaterial = nullptr;
	}
	Material::~Material()
	{

	}



	// Creation/Destruction: (register/delete from MaterialManager)
	Material Material::CreateForward(emberCommon::RenderMode renderMode, const std::string& name, int32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		return MaterialManager::CreateForwardMaterial(renderMode, name, renderQueue, vertexSpv, fragmentSpv);
	}
	Material Material::CreateShadow(const std::string& name, const std::filesystem::path& vertexSpv)
	{
		return MaterialManager::CreateShadowMaterial(name, vertexSpv);
	}
	void Material::Destroy()
	{
		if (!IsValid())
		{
			LOG_WARN("Attempting to destroy invalid material");
			return;
		}
		MaterialManager::DeleteMaterial(GetName());
	}



	// Setters:
	void Material::SetRenderQueue(int32_t renderQueue)
	{
		m_pIMaterial->SetRenderQueue(renderQueue);
	}
	void Material::SetRenderMode(emberCommon::RenderMode renderMode)
	{
		m_pIMaterial->SetRenderMode(renderMode);
	}
	void Material::SetShadowMaterial(const Material& shadowMaterial)
	{
		m_pIMaterial->SetShadowMaterial(shadowMaterial.GetInterfaceHandle());
	}



	// Getters:
	const std::string& Material::GetName() const
	{
		return m_pIMaterial->GetName();
	}
	int32_t Material::GetRenderQueue() const
	{
		return m_pIMaterial->GetRenderQueue();
	}
	emberCommon::RenderMode Material::GetRenderMode() const
	{
		return m_pIMaterial->GetRenderMode();
	}
	Material Material::GetShadowMaterial() const
	{
		emberBackendInterface::IMaterial* pIShadowMaterial = m_pIMaterial->GetShadowMaterial();
        if (pIShadowMaterial == nullptr)
			throw std::runtime_error(GetName() + "has no shadow material not found.");
		return Material{ pIShadowMaterial };
	}
	Material Material::TryGetShadowMaterial() const
	{
		emberBackendInterface::IMaterial* pIShadowMaterial = m_pIMaterial->GetShadowMaterial();
        if (pIShadowMaterial == nullptr)
        {
			LOG_ERROR("Material '{}' has no shadow material!", GetName());
            return Material();
        }
		return Material{ pIShadowMaterial };
	}
	bool Material::IsValid() const
	{
		return m_pIMaterial != nullptr;
	}



	// Debugging:
	void Material::Print() const
	{
		m_pIMaterial->Print();
	}
}