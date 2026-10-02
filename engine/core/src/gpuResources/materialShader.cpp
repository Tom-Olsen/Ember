#include "materialShader.h"
#include "iMaterialShader.h"
#include "materialShaderManager.h"
#include <stdexcept>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	MaterialShader::MaterialShader()
		: m_materialShaderId(emberCommon::invalidMaterialShaderId)
	{

	}
	MaterialShader::~MaterialShader() = default;



	// Getters:
	const std::string& MaterialShader::GetName() const
	{
		const std::string* pName = MaterialShaderManager::TryGetMaterialShaderName(m_materialShaderId);
		if (pName == nullptr)
			throw std::runtime_error("MaterialShader::GetName() failed. MaterialShader is invalid or expired.");
		return *pName;
	}
	emberCommon::MaterialPass MaterialShader::GetMaterialPass() const
	{
		emberBackendInterface::IMaterialShader* pMaterialShader = TryGetInterfaceHandle();
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialShader::GetMaterialPass() failed. MaterialShader is invalid or expired.");
		return pMaterialShader->GetMaterialPass();
	}
	bool MaterialShader::IsValid() const
	{
		return TryGetInterfaceHandle() != nullptr;
	}



	// Debugging:
	void MaterialShader::Print() const
	{
		emberBackendInterface::IMaterialShader* pMaterialShader = TryGetInterfaceHandle();
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialShader::Print() failed. MaterialShader is invalid or expired.");
		pMaterialShader->Print();
	}



	// Private methods:
	MaterialShader::MaterialShader(emberCommon::MaterialShaderId materialShaderId)
		: m_materialShaderId(materialShaderId)
	{

	}
	emberBackendInterface::IMaterialShader* MaterialShader::TryGetInterfaceHandle() const
	{
		return MaterialShaderManager::TryGetMaterialShaderInterface(m_materialShaderId);
	}
}