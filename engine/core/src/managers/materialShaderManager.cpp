#include "materialShaderManager.h"
#include "logger.h"
#include "materialManager.h"
#include <stdexcept>



namespace emberCore
{
	// Static members:
	bool MaterialShaderManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<MaterialShader>> MaterialShaderManager::s_materialShaders;



	// Initialization/Cleanup:
	void MaterialShaderManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
	}
	void MaterialShaderManager::Clear()
	{
		for (const auto& pair : s_materialShaders)
		{
			if (MaterialManager::MaterialShaderInUse(pair.first))
				throw std::runtime_error("MaterialShaderManager::Clear() failed. MaterialShader is still used by at least one material: " + pair.first);
		}
		s_materialShaders.clear();
		s_isInitialized = false;
	}



	// Creators:
	MaterialShader& MaterialShaderManager::CreateOutlineMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader* pMaterialShader = TryGetMaterialShader(name);
		if (pMaterialShader != nullptr)
		{
			if (pMaterialShader->GetMaterialType() != emberCommon::MaterialType::outline)
				throw std::runtime_error("MaterialShaderManager::CreateOutlineMaterialShader(...) failed. Existing MaterialShader is not an outline shader: " + name);
			return *pMaterialShader;
		}

		AddMaterialShader(MaterialShader::CreateOutline(vertexSpv, fragmentSpv, name));
		return GetMaterialShader(name);
	}
	MaterialShader& MaterialShaderManager::CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader* pMaterialShader = TryGetMaterialShader(name);
		if (pMaterialShader != nullptr)
		{
			if (pMaterialShader->GetMaterialType() != emberCommon::MaterialType::forward)
				throw std::runtime_error("MaterialShaderManager::CreateForwardMaterialShader(...) failed. Existing MaterialShader is not a forward shader: " + name);
			return *pMaterialShader;
		}

		AddMaterialShader(MaterialShader::CreateForward(vertexSpv, fragmentSpv, name));
		return GetMaterialShader(name);
	}
	MaterialShader& MaterialShaderManager::CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader* pMaterialShader = TryGetMaterialShader(name);
		if (pMaterialShader != nullptr)
		{
			if (pMaterialShader->GetMaterialType() != emberCommon::MaterialType::gizmo)
				throw std::runtime_error("MaterialShaderManager::CreateGizmoMaterialShader(...) failed. Existing MaterialShader is not a gizmo shader: " + name);
			return *pMaterialShader;
		}

		AddMaterialShader(MaterialShader::CreateGizmo(vertexSpv, fragmentSpv, name));
		return GetMaterialShader(name);
	}
	MaterialShader& MaterialShaderManager::CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		MaterialShader* pMaterialShader = TryGetMaterialShader(name);
		if (pMaterialShader != nullptr)
		{
			if (pMaterialShader->GetMaterialType() != emberCommon::MaterialType::shadow)
				throw std::runtime_error("MaterialShaderManager::CreateShadowMaterialShader(...) failed. Existing MaterialShader is not a shadow shader: " + name);
			return *pMaterialShader;
		}

		AddMaterialShader(MaterialShader::CreateShadow(vertexSpv, name));
		return GetMaterialShader(name);
	}
	MaterialShader& MaterialShaderManager::CreatePresentMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader* pMaterialShader = TryGetMaterialShader(name);
		if (pMaterialShader != nullptr)
		{
			if (pMaterialShader->GetMaterialType() != emberCommon::MaterialType::present)
				throw std::runtime_error("MaterialShaderManager::CreatePresentMaterialShader(...) failed. Existing MaterialShader is not a present shader: " + name);
			return *pMaterialShader;
		}

		AddMaterialShader(MaterialShader::CreatePresent(vertexSpv, fragmentSpv, name));
		return GetMaterialShader(name);
	}



	// Add/Get/Delete:
	void MaterialShaderManager::AddMaterialShader(MaterialShader&& materialShader)
	{
		std::unique_ptr<MaterialShader> newMaterialShader = std::make_unique<MaterialShader>(std::move(materialShader));
		const std::string name = newMaterialShader->GetName();
		if (!s_materialShaders.emplace(name, std::move(newMaterialShader)).second)
			LOG_WARN("MaterialShader with the name: {} already exists in MaterialShaderManager!", name);
	}
	MaterialShader& MaterialShaderManager::GetMaterialShader(const std::string& name)
	{
		auto it = s_materialShaders.find(name);
		if (it == s_materialShaders.end())
			throw std::runtime_error("MaterialShader not found: " + name);
		return *(it->second);
	}
	MaterialShader* MaterialShaderManager::TryGetMaterialShader(const std::string& name)
	{
		auto it = s_materialShaders.find(name);
		if (it != s_materialShaders.end())
			return it->second.get();
		return nullptr;
	}
	void MaterialShaderManager::DeleteMaterialShader(const std::string& name)
	{
		if (MaterialManager::MaterialShaderInUse(name))
			throw std::runtime_error("MaterialShaderManager::DeleteMaterialShader(...) failed. MaterialShader is still used by at least one material: " + name);
		s_materialShaders.erase(name);
	}



	// Debugging:
	void MaterialShaderManager::Print()
	{
		LOG_TRACE("MaterialShaderManager content:");
		for (const auto& pair : s_materialShaders)
			LOG_TRACE(pair.first);
	}
}