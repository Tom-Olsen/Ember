#include "materialShader.h"
#include "iMaterialShader.h"
#include "logger.h"
#include "materialShaderManager.h"



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	MaterialShader::MaterialShader()
		: m_materialShaderId(emberCommon::invalidMaterialShaderId)
	{

	}
	MaterialShader::~MaterialShader()
	{

	}



	// Creation/Destruction:
	MaterialShader MaterialShader::CreateGizmo(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		return MaterialShaderManager::CreateGizmoMaterialShader(vertexSpv, fragmentSpv, name);
	}
	MaterialShader MaterialShader::CreateShadow(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		return MaterialShaderManager::CreateShadowMaterialShader(vertexSpv, name);
	}
	MaterialShader MaterialShader::CreateDeferredGeometry(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		return MaterialShaderManager::CreateDeferredGeometryMaterialShader(vertexSpv, fragmentSpv, name);
	}
	MaterialShader MaterialShader::CreateForward(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		return MaterialShaderManager::CreateForwardMaterialShader(vertexSpv, fragmentSpv, name);
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
	emberCommon::MaterialPass MaterialShader::GetMaterialPass() const
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
		{
			LOG_WARN("MaterialShader::GetMaterialPass() failed. MaterialShader is invalid or expired.");
			return emberCommon::MaterialPass::count;
		}
		return pIMaterialShader->GetMaterialPass();
	}
	bool MaterialShader::IsValid() const
	{
		return GetInterfaceHandle() != nullptr;
	}



	// Private methods:
	MaterialShader::MaterialShader(emberCommon::MaterialShaderId materialShaderId)
		: m_materialShaderId(materialShaderId)
	{

	}
	emberBackendInterface::IMaterialShader* MaterialShader::GetInterfaceHandle() const
	{
		return MaterialShaderManager::GetMaterialShaderInterface(m_materialShaderId);
	}
}