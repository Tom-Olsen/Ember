#include "materialShader.h"
#include "logger.h"
#include "materialShaderManager.h"



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	MaterialShader::MaterialShader()
		: m_materialShaderId(invalidMaterialShaderId)
	{

	}
	MaterialShader::~MaterialShader()
	{

	}



	// Creation/Destruction:
	MaterialShader MaterialShader::CreateForward(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		return MaterialShaderManager::CreateForwardMaterialShader(vertexSpv, fragmentSpv, name);
	}
	MaterialShader MaterialShader::CreateGizmo(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		return MaterialShaderManager::CreateGizmoMaterialShader(vertexSpv, fragmentSpv, name);
	}
	MaterialShader MaterialShader::CreateShadow(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		return MaterialShaderManager::CreateShadowMaterialShader(vertexSpv, name);
	}
	void MaterialShader::Destroy()
	{
		if (!IsValid())
		{
			LOG_WARN("Attempting to destroy invalid material shader");
			return;
		}
		MaterialShaderManager::DeleteMaterialShader(m_materialShaderId);
	}



	// Getters:
	const std::string& MaterialShader::GetName() const
	{
		const std::string* pName = MaterialShaderManager::GetMaterialShaderName(m_materialShaderId);
		if (pName == nullptr)
		{
			LOG_WARN("MaterialShader::GetName() failed. MaterialShader is invalid or expired.");
			static const std::string emptyName;
			return emptyName;
		}
		return *pName;
	}
	emberCommon::MaterialType MaterialShader::GetMaterialType() const
	{
		const emberCommon::MaterialType* pMaterialType = MaterialShaderManager::GetMaterialShaderType(m_materialShaderId);
		if (pMaterialType == nullptr)
		{
			LOG_WARN("MaterialShader::GetMaterialType() failed. MaterialShader is invalid or expired.");
			return emberCommon::MaterialType::count;
		}
		return *pMaterialType;
	}
	bool MaterialShader::IsValid() const
	{
		return GetInterfaceHandle() != nullptr;
	}



	// Private methods:
	MaterialShader::MaterialShader(MaterialShaderId materialShaderId)
		: m_materialShaderId(materialShaderId)
	{

	}
	emberBackendInterface::IMaterialShader* MaterialShader::GetInterfaceHandle() const
	{
		return MaterialShaderManager::GetMaterialShaderInterface(m_materialShaderId);
	}
}