#include "shadowMaterial.h"
#include "iMaterial.h"
#include "materialManager.h"
#include <stdexcept>



namespace emberCore
{
	// Private methods:
	ShadowMaterial::ShadowMaterial(MaterialId materialId)
		: Material(materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("ShadowMaterial::ShadowMaterial(...) failed. Material is invalid or expired.");
		if (pIMaterial->GetMaterialType() != emberCommon::MaterialType::shadow)
			throw std::runtime_error("ShadowMaterial::ShadowMaterial(...) failed. IMaterial is not a shadow material.");
	}



	// Public methods:
	// Constructor/Destructor:
	ShadowMaterial::ShadowMaterial()
		: Material()
	{

	}
	ShadowMaterial::~ShadowMaterial()
	{

	}



	// Cloning:
	ShadowMaterial ShadowMaterial::Clone(const std::string& name) const
	{
		return MaterialManager::CloneShadowMaterial(*this, name);
	}
}