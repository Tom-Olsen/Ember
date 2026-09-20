#include "computeShaderManager.h"
#include "computeShaderAsset.h"
#include "computeShaderAssetLoader.h"
#include "iComputeShaderManager.h"
#include "iGpuResourceFactory.h"
#include "logger.h"
#include "renderer.h"
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <vector>



namespace emberCore
{
	// Static members:
	emberBackendInterface::IComputeShaderManager* ComputeShaderManager::s_pIComputeShaderManager = nullptr;



	// Public methods:
	// Asset loading:
	void ComputeShaderManager::LoadComputeShaderAssets(const std::filesystem::path& directoryPath)
	{
		if (s_pIComputeShaderManager == nullptr)
			throw std::runtime_error("ComputeShaderManager::LoadComputeShaderAssets(...) failed. Compute shader manager is not initialized.");
		if (!std::filesystem::is_directory(directoryPath))
			throw std::runtime_error("ComputeShaderManager::LoadComputeShaderAssets(...) failed. Directory does not exist: " + directoryPath.string());

		std::vector<std::filesystem::path> assetPaths;
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_regular_file() && entry.path().filename().string().ends_with(".computeShaderAsset.json"))
				assetPaths.push_back(entry.path());
		}
		std::sort(assetPaths.begin(), assetPaths.end());

		std::vector<emberAssetLoader::ComputeShaderAsset> computeShaderAssets;
		computeShaderAssets.reserve(assetPaths.size());
		for (const std::filesystem::path& assetPath : assetPaths)
			computeShaderAssets.push_back(emberAssetLoader::ComputeShaderAssetLoader::Load(assetPath));
		for (const emberAssetLoader::ComputeShaderAsset& computeShaderAsset : computeShaderAssets)
			s_pIComputeShaderManager->CreateComputeShader(computeShaderAsset);
	}



	// Getters:
	ComputeShader ComputeShaderManager::TryGetComputeShader(const std::string& name)
	{
		if (s_pIComputeShaderManager == nullptr)
		{
			LOG_WARN("ComputeShaderManager::TryGetComputeShader(...) failed. Compute shader manager is not initialized.");
			return ComputeShader();
		}
		emberCommon::ComputeShaderId computeShaderId = s_pIComputeShaderManager->TryGetComputeShaderId(name);
		if (computeShaderId.index == emberCommon::invalidComputeShaderId.index)
		{
			LOG_WARN("ComputeShaderManager::TryGetComputeShader(...) failed. ComputeShader '{}' not found or inaccessible.", name);
			return ComputeShader();
		}
		return ComputeShader(computeShaderId);
	}



	// Debugging:
	void ComputeShaderManager::Print()
	{
		s_pIComputeShaderManager->Print();
	}



	// Private methods:
	// Initialization/Cleanup:
	void ComputeShaderManager::Init()
	{
		if (s_pIComputeShaderManager != nullptr)
			return;
		if (Renderer::s_pIGpuResourceFactory == nullptr)
			throw std::runtime_error("ComputeShaderManager::Init() failed. Gpu resource factory is not initialized.");

		s_pIComputeShaderManager = Renderer::s_pIGpuResourceFactory->GetComputeShaderManager();
		if (s_pIComputeShaderManager == nullptr)
			throw std::runtime_error("ComputeShaderManager::Init() failed. Gpu resource factory returned a nullptr compute shader manager.");

		LoadComputeShaderAssets(std::filesystem::path(ENGINE_SHADERS_DIR) / "computeShaderAssets");
		s_pIComputeShaderManager->InitializeDefaultComputeShaders();
	}
	void ComputeShaderManager::Clear()
	{
		s_pIComputeShaderManager = nullptr;
	}



	// Getters:
	bool ComputeShaderManager::IsComputeShaderMutable(emberCommon::ComputeShaderId computeShaderId)
	{
		return s_pIComputeShaderManager != nullptr && s_pIComputeShaderManager->IsComputeShaderMutable(computeShaderId);
	}
	emberBackendInterface::IComputeShader* ComputeShaderManager::TryGetComputeShaderInterface(emberCommon::ComputeShaderId computeShaderId)
	{
		if (s_pIComputeShaderManager == nullptr)
			return nullptr;
		return s_pIComputeShaderManager->TryGetComputeShader(computeShaderId);
	}
	const std::string* ComputeShaderManager::TryGetComputeShaderName(emberCommon::ComputeShaderId computeShaderId)
	{
		if (s_pIComputeShaderManager == nullptr)
			return nullptr;
		return s_pIComputeShaderManager->TryGetComputeShaderName(computeShaderId);
	}
}