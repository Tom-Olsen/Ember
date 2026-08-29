#include "shadowMaterial.h"
#include "iMaterial.h"
#include "materialManager.h"
#include <stdexcept>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	ShadowMaterial::ShadowMaterial()
		: Material()
	{

	}
	ShadowMaterial::~ShadowMaterial()
	{

	}



	// Creation/Cloning:
	ShadowMaterial ShadowMaterial::CreateFromShader(const MaterialShader& materialShader, const std::string& name)
	{
		return MaterialManager::CreateShadowMaterial(materialShader, name);
	}
	ShadowMaterial ShadowMaterial::Clone(const std::string& name) const
	{
		return MaterialManager::CloneShadowMaterial(*this, name);
	}



	// Private methods:
	ShadowMaterial::ShadowMaterial(emberCommon::MaterialId materialId)
		: Material(materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetInterfaceHandle();
		if (pIMaterial == nullptr)
			throw std::runtime_error("ShadowMaterial::ShadowMaterial(...) failed. Material is invalid or expired.");
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("ShadowMaterial::ShadowMaterial(...) failed. IMaterial is not a shadow material.");
	}
}