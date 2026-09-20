#include "material.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialManager.h"
#include <stdexcept>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	Material::Material()
	{
		m_materialId = emberCommon::invalidMaterialId;
	}
	Material::~Material()
	{

	}



	// Destruction:
	void Material::Destroy()
	{
		if (!IsValid())
		{
			LOG_WARN("Attempting to destroy invalid material");
			return;
		}
		MaterialManager::DeleteMaterial(m_materialId);
	}



	// Getters:
	const std::string& Material::GetName() const
	{
		const std::string* pName = MaterialManager::TryGetMaterialName(m_materialId);
		if (pName == nullptr)
		{
			LOG_WARN("Material::GetName() failed. Material is invalid or expired.");
			static const std::string invalidName = "invalidOrExpired";
			return invalidName;
		}
		return *pName;
	}
	emberCommon::MaterialPass Material::GetMaterialPass() const
	{
		emberBackendInterface::IMaterial* pIMaterial = TryGetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("Material::GetMaterialPass() failed. Material is invalid or expired.");
			return emberCommon::MaterialPass::count;
		}
		return pIMaterial->GetMaterialPass();
	}
	emberCommon::CullMode Material::GetCullMode() const
	{
		emberBackendInterface::IMaterial* pIMaterial = TryGetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("Material::GetCullMode() failed. Material is invalid or expired.");
			return emberCommon::CullMode::count;
		}
		return pIMaterial->GetCullMode();
	}
	bool Material::IsValid() const
	{
		return TryGetInterfaceHandle() != nullptr;
	}



	// Debugging:
	void Material::Print() const
	{
		emberBackendInterface::IMaterial* pIMaterial = TryGetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("Material::Print() failed. Material is invalid or expired.");
		pIMaterial->Print();
	}



	// Protected methods:
	Material::Material(emberCommon::MaterialId materialId)
		: Shader()
	{
		m_materialId = materialId;
	}
	emberBackendInterface::IMaterial* Material::TryGetInterfaceHandle() const
	{
		return MaterialManager::TryGetMaterialInterface(m_materialId);
	}
	emberBackendInterface::IMaterial* Material::TryGetMutableInterfaceHandle() const
	{
		emberBackendInterface::IMaterial* pIMaterial = TryGetInterfaceHandle();
		if (pIMaterial == nullptr || !MaterialManager::IsMaterialMutable(m_materialId))
			return nullptr;
		return pIMaterial;
	}
	emberBackendInterface::IDescriptorSetBinding* Material::TryGetShaderDescriptorSetBinding() const
	{
		if (emberBackendInterface::IMaterial* pIMaterial = TryGetMutableInterfaceHandle())
			return pIMaterial->GetShaderDescriptorSetBinding();
		return nullptr;
	}
}