#include "shadowMaterial.h"
#include "iMaterial.h"
#include <stdexcept>



namespace emberCore
{
	// Private methods:
	ShadowMaterial::ShadowMaterial(emberBackendInterface::IMaterial* pIMaterial)
		: Material(pIMaterial)
	{
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
		return MaterialManager::CloneShadowMaterial(name, *this);
	}
}