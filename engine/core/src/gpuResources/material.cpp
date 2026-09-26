#include "material.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialManager.h"
#include "shadowMaterial.h"
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
			pIMaterial = MaterialManager::s_pIErrorMaterial;
		}
		return pIMaterial->GetMaterialPass();
	}
	emberCommon::CullMode Material::GetCullMode() const
	{
		emberBackendInterface::IMaterial* pIMaterial = TryGetInterfaceHandle();
		if (pIMaterial == nullptr)
		{
			LOG_WARN("Material::GetCullMode() failed. Material is invalid or expired.");
			pIMaterial = MaterialManager::s_pIErrorMaterial;
		}
		return pIMaterial->GetCullMode();
	}
	ShadowMaterial Material::GetShadowMaterial() const
	{
		emberCommon::MaterialId shadowMaterialId = MaterialManager::TryGetShadowMaterialIdOfSurfaceMaterial(m_materialId);
		if (shadowMaterialId.index == emberCommon::invalidMaterialId.index)
			return ShadowMaterial();
		return ShadowMaterial{ shadowMaterialId };
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
		{
			LOG_WARN("Material::Print() failed. Material is invalid or expired.");
			return;
		}
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
		{
			LOG_WARN("Material::TryGetMutableInterfaceHandle() failed. Material is invalid or expired.");
			pIMaterial = MaterialManager::s_pIErrorMaterial;
		}
		return pIMaterial;
	}
	emberBackendInterface::IDescriptorSetBinding* Material::TryGetShaderDescriptorSetBinding() const
	{
		if (emberBackendInterface::IMaterial* pIMaterial = TryGetMutableInterfaceHandle())
			return pIMaterial->GetShaderDescriptorSetBinding();
		return nullptr;
	}
}