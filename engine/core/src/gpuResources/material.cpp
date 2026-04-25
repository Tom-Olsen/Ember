#include "material.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialManager.h"



namespace emberEngine
{
	// Private methods:
	emberBackendInterface::IMaterial* const Material::GetInterfaceHandle() const
	{
		return m_pIMaterial;
	}
	Material::Material(emberBackendInterface::IMaterial* pIMaterial)
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
	Material Material::Create(emberCommon::RenderMode renderMode, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		return MaterialManager::CreateMaterial(renderMode, name, renderQueue, vertexSpv, fragmentSpv);
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
	void Material::SetRenderQueue(uint32_t renderQueue)
	{
		m_pIMaterial->SetRenderQueue(renderQueue);
	}
	void Material::SetRenderMode(emberCommon::RenderMode renderMode)
	{
		m_pIMaterial->SetRenderMode(renderMode);
	}



	// Getters:
	const std::string& Material::GetName() const
	{
		return m_pIMaterial->GetName();
	}
	uint32_t Material::GetRenderQueue() const
	{
		return m_pIMaterial->GetRenderQueue();
	}
	emberCommon::RenderMode Material::GetRenderMode() const
	{
		return m_pIMaterial->GetRenderMode();
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