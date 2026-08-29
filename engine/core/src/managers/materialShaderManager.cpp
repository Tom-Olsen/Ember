#include "materialShaderManager.h"
#include "iMaterialShaderManager.h"
#include "iRenderer.h"
#include "logger.h"
#include "renderer.h"
#include <stdexcept>



namespace emberCore
{
	// Static members:
	std::unique_ptr<emberBackendInterface::IMaterialShaderManager> MaterialShaderManager::s_pIMaterialShaderManager;



	// Public methods:
	// Creators:
	MaterialShader MaterialShaderManager::CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		return MaterialShader{ s_pIMaterialShaderManager->CreateGizmoMaterialShader(vertexSpv, fragmentSpv, name) };
	}
	MaterialShader MaterialShaderManager::CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		return MaterialShader{ s_pIMaterialShaderManager->CreateShadowMaterialShader(vertexSpv, name) };
	}
	MaterialShader MaterialShaderManager::CreateDeferredGeometryMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		return MaterialShader{ s_pIMaterialShaderManager->CreateDeferredGeometryMaterialShader(vertexSpv, fragmentSpv, name) };
	}
	MaterialShader MaterialShaderManager::CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		return MaterialShader{ s_pIMaterialShaderManager->CreateForwardMaterialShader(vertexSpv, fragmentSpv, name) };
	}



	// Getters:
	MaterialShader MaterialShaderManager::GetMaterialShader(const std::string& name)
	{
		emberCommon::MaterialShaderId materialShaderId = GetMaterialShaderId(name);
		if (GetMaterialShaderInterface(materialShaderId) == nullptr)
		{
			LOG_WARN("MaterialShaderManager::GetMaterialShader(...) failed. MaterialShader '{}' not found or expired.", name);
			return MaterialShader();
		}
		return MaterialShader{ materialShaderId };
	}



	// Deleter:
	void MaterialShaderManager::DeleteMaterialShader(const std::string& name)
	{
		DeleteMaterialShader(GetMaterialShaderId(name));
	}



	// Debugging:
	void MaterialShaderManager::Print()
	{
		s_pIMaterialShaderManager->Print();
	}



	// Private methods:
	// Initialization/Cleanup:
	void MaterialShaderManager::Init()
	{
		if (s_pIMaterialShaderManager != nullptr)
			return;
		if (Renderer::s_pIRenderer == nullptr)
			throw std::runtime_error("MaterialShaderManager::Init() failed. Renderer is not initialized.");

		s_pIMaterialShaderManager.reset(Renderer::s_pIRenderer->CreateMaterialShaderManager());
		if (s_pIMaterialShaderManager == nullptr)
			throw std::runtime_error("MaterialShaderManager::Init() failed. Renderer returned a nullptr material shader manager.");
	}
	void MaterialShaderManager::Clear()
	{
		s_pIMaterialShaderManager.reset();
	}



	// Getters:
	emberCommon::MaterialShaderId MaterialShaderManager::GetMaterialShaderId(const std::string& name)
	{
		return s_pIMaterialShaderManager->TryGetMaterialShaderId(name);
	}
	emberBackendInterface::IMaterialShaderManager* MaterialShaderManager::GetInterfaceHandle()
	{
		if (s_pIMaterialShaderManager == nullptr)
			throw std::runtime_error("MaterialShaderManager::GetInterfaceHandle() failed. Material shader manager is not initialized.");
		return s_pIMaterialShaderManager.get();
	}
	emberBackendInterface::IMaterialShader* MaterialShaderManager::GetMaterialShaderInterface(emberCommon::MaterialShaderId materialShaderId)
	{
		return s_pIMaterialShaderManager->TryGetMaterialShader(materialShaderId);
	}
	const std::string* MaterialShaderManager::GetMaterialShaderName(emberCommon::MaterialShaderId materialShaderId)
	{
		return s_pIMaterialShaderManager->TryGetMaterialShaderName(materialShaderId);
	}



	// Deleter:
	void MaterialShaderManager::DeleteMaterialShader(emberCommon::MaterialShaderId materialShaderId)
	{
		s_pIMaterialShaderManager->DeleteMaterialShader(materialShaderId);
	}
}