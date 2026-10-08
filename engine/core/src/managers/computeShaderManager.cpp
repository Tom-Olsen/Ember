#include "computeShaderManager.h"
#include "commonComputeShaderCreateInfo.h"
#include "computeShaderAsset.h"
#include "computeShaderAssetLoader.h"
#include "defaultGpuResources.h"
#include "gpuResourceFactory.h"
#include "iComputeShader.h"
#include "logger.h"
#include <algorithm>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>



namespace emberCore
{
	// Static members:
	bool ComputeShaderManager::s_isInitialized = false;
	emberDataStructures::NamedSlotMap<emberCommon::ComputeShaderId, ComputeShaderManager::ManagedComputeShader> ComputeShaderManager::s_computeShaderSlotMap;



	// Managed compute shader methods:
	ComputeShaderManager::ManagedComputeShader::ManagedComputeShader()
		: accessRights{ false, false, false }
	{

	}
	ComputeShaderManager::ManagedComputeShader::ManagedComputeShader(const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader)
		: accessRights(accessRights)
		, pComputeShader(std::move(pComputeShader))
	{

	}
	ComputeShaderManager::ManagedComputeShader::~ManagedComputeShader() = default;
	ComputeShaderManager::ManagedComputeShader::ManagedComputeShader(ManagedComputeShader&&) noexcept = default;
	ComputeShaderManager::ManagedComputeShader& ComputeShaderManager::ManagedComputeShader::operator=(ManagedComputeShader&&) noexcept = default;



	// Public methods:
	// Asset loading:
	void ComputeShaderManager::LoadComputeShaderAssets(const std::filesystem::path& directoryPath)
	{
		// Error handling:
		if (!GpuResourceFactory::s_isInitialized)
			throw std::runtime_error("ComputeShaderManager::LoadComputeShaderAssets(...) failed. Compute shader manager is not initialized.");
		if (!std::filesystem::is_directory(directoryPath))
			throw std::runtime_error("ComputeShaderManager::LoadComputeShaderAssets(...) failed. Directory does not exist: " + directoryPath.string());

		// Collect asset paths:
		std::vector<std::filesystem::path> assetPaths;
		for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(directoryPath))
		{
			if (directoryEntry.is_regular_file() && directoryEntry.path().filename().string().ends_with(".computeShaderAsset.json"))
				assetPaths.push_back(directoryEntry.path());
		}
		std::sort(assetPaths.begin(), assetPaths.end());

		// Load compute shader assets:
		std::vector<emberAssetLoader::ComputeShaderAsset> computeShaderAssets;
		computeShaderAssets.reserve(assetPaths.size());
		for (const std::filesystem::path& assetPath : assetPaths)
			computeShaderAssets.push_back(emberAssetLoader::ComputeShaderAssetLoader::Load(assetPath));

		// Create compute shaders:
		for (const emberAssetLoader::ComputeShaderAsset& computeShaderAsset : computeShaderAssets)
			CreateComputeShader(computeShaderAsset);
	}



	// Getters:
	ComputeShader ComputeShaderManager::TryGetComputeShader(const std::string& name)
	{
		if (!s_isInitialized)
		{
			LOG_WARN("ComputeShaderManager::TryGetComputeShader(...) failed. Compute shader manager is not initialized.");
			return ComputeShader();
		}

		emberCommon::ComputeShaderId computeShaderId = TryGetAccessibleComputeShaderId(name);
		if (computeShaderId.index == emberCommon::invalidComputeShaderId.index)
		{
			LOG_WARN("ComputeShaderManager::TryGetComputeShader(...) failed. ComputeShader '{}' not found or inaccessible.", name);
			return ComputeShader();
		}
		return ComputeShader(computeShaderId);
	}



	// Deleter:
	void ComputeShaderManager::DeleteComputeShader(const std::string& name)
	{
		emberCommon::ComputeShaderId computeShaderId = FindComputeShaderId(name);
		if (TryGetComputeShaderInterface(computeShaderId) == nullptr)
		{
			LOG_WARN("ComputeShaderManager::DeleteComputeShader(...) failed. ComputeShader '{}' not found.", name);
			return;
		}
		DeleteComputeShader(computeShaderId);
	}



	// Debugging:
	void ComputeShaderManager::Print()
	{
		if (!s_isInitialized)
		{
			LOG_WARN("ComputeShaderManager::Print() failed. Compute shader manager is not initialized.");
			return;
		}

		LOG_TRACE("ComputeShaderManager contents:");
		for (emberCommon::ComputeShaderId computeShaderId : s_computeShaderSlotMap.GetActiveIds())
		{
			const ManagedComputeShader* pManagedComputeShader = s_computeShaderSlotMap.TryGetValue(computeShaderId);
			const emberCommon::ResourceAccessRights& accessRights = pManagedComputeShader->accessRights;
			std::string name = GetComputeShaderName(computeShaderId);
			LOG_TRACE("  {}: index {}, generation {}, accessible {}, deletable {}, mutable {}", name, computeShaderId.index, computeShaderId.generation, accessRights.isAccessible, accessRights.isDeletable, accessRights.isMutable);
		}
	}



	// Private methods:
	// Initialization/Cleanup:
	void ComputeShaderManager::Init()
	{
		if (s_isInitialized)
			return;
		if (!GpuResourceFactory::s_isInitialized)
			throw std::runtime_error("ComputeShaderManager::Init() failed. Gpu resource factory is not initialized.");

		LoadComputeShaderAssets(std::filesystem::path(ENGINE_SHADERS_DIR) / "computeShaderAssets");
		DefaultGpuResources::InitializeBuiltInComputeShaders(
			TakeComputeShaderOwnership("gammaCorrection"),
			TakeComputeShaderOwnership("outlineComposite"),
			TakeComputeShaderOwnership("outlineHorizontalMaskExpansion"),
			TakeComputeShaderOwnership("outlineVerticalMaskExpansion"));
		s_isInitialized = true;
	}
	void ComputeShaderManager::Clear()
	{
		if (!s_isInitialized)
			return;

		for (emberCommon::ComputeShaderId computeShaderId : s_computeShaderSlotMap.GetActiveIds())
		{
			std::optional<ManagedComputeShader> managedComputeShader = s_computeShaderSlotMap.Remove(computeShaderId);
			RetireComputeShader(std::move(managedComputeShader->pComputeShader));
		}
		s_isInitialized = false;
	}



	// Creation:
	emberCommon::ComputeShaderId ComputeShaderManager::CreateComputeShader(const emberAssetLoader::ComputeShaderAsset& computeShaderAsset)
	{
		// Compute shader with that name already exists:
		emberCommon::ComputeShaderId computeShaderId = FindComputeShaderId(computeShaderAsset.computeShaderName);
		if (TryGetComputeShaderInterface(computeShaderId) != nullptr)
			return computeShaderId;

		// Create compute shader from asset:
		emberCommon::ComputeShaderCreateInfo computeShaderCreateInfo
		{
			computeShaderAsset.binaryPath,
			computeShaderAsset.features,
			computeShaderAsset.computeShaderName
		};
		std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader(GpuResourceFactory::CreateComputeShader(computeShaderCreateInfo));
		if (pComputeShader == nullptr)
			throw std::runtime_error("ComputeShaderManager::CreateComputeShader(...) failed. Gpu resource factory returned nullptr for: " + computeShaderAsset.computeShaderName);

		return AddComputeShader(computeShaderAsset.computeShaderName, computeShaderAsset.accessRights, std::move(pComputeShader));
	}



	// Getters:
	emberCommon::ComputeShaderId ComputeShaderManager::TryGetAccessibleComputeShaderId(const std::string& name)
	{
		emberCommon::ComputeShaderId computeShaderId = FindComputeShaderId(name);
		const ManagedComputeShader* pManagedComputeShader = s_computeShaderSlotMap.TryGetValue(computeShaderId);
		if (pManagedComputeShader == nullptr || !pManagedComputeShader->accessRights.isAccessible)
			return emberCommon::invalidComputeShaderId;
		return computeShaderId;
	}
	bool ComputeShaderManager::IsComputeShaderMutable(emberCommon::ComputeShaderId computeShaderId)
	{
		const ManagedComputeShader* pManagedComputeShader = s_computeShaderSlotMap.TryGetValue(computeShaderId);
		return pManagedComputeShader != nullptr && pManagedComputeShader->accessRights.isMutable;
	}
	emberBackendInterface::IComputeShader* ComputeShaderManager::TryGetComputeShaderInterface(emberCommon::ComputeShaderId computeShaderId)
	{
		ManagedComputeShader* pManagedComputeShader = s_computeShaderSlotMap.TryGetValue(computeShaderId);
		return pManagedComputeShader != nullptr ? pManagedComputeShader->pComputeShader.get() : nullptr;
	}
	std::string ComputeShaderManager::GetComputeShaderName(emberCommon::ComputeShaderId computeShaderId)
	{
		std::optional<std::string> name = s_computeShaderSlotMap.TryGetName(computeShaderId);
		if (!name)
			throw std::runtime_error("ComputeShaderManager::GetComputeShaderName(...) failed. ComputeShader is invalid or expired.");
		return std::move(*name);
	}



	// Ownership transfer:
	std::unique_ptr<emberBackendInterface::IComputeShader> ComputeShaderManager::TakeComputeShaderOwnership(const std::string& name)
	{
		emberCommon::ComputeShaderId computeShaderId = FindComputeShaderId(name);
		std::optional<ManagedComputeShader> managedComputeShader = s_computeShaderSlotMap.Remove(computeShaderId);
		if (!managedComputeShader)
			throw std::runtime_error("ComputeShaderManager::TakeComputeShaderOwnership(...) failed. ComputeShader not found: " + name);
		return std::move(managedComputeShader->pComputeShader);
	}



	// Deleter:
	void ComputeShaderManager::DeleteComputeShader(emberCommon::ComputeShaderId computeShaderId)
	{
		ManagedComputeShader* pManagedComputeShader = s_computeShaderSlotMap.TryGetValue(computeShaderId);
		if (pManagedComputeShader == nullptr)
			return;
		if (!pManagedComputeShader->accessRights.isDeletable)
		{
			LOG_WARN("ComputeShaderManager::DeleteComputeShader(...) failed. ComputeShader '{}' is not deletable.", GetComputeShaderName(computeShaderId));
			return;
		}
		std::optional<ManagedComputeShader> managedComputeShader = s_computeShaderSlotMap.Remove(computeShaderId);
		RetireComputeShader(std::move(managedComputeShader->pComputeShader));
	}



	// Management:
	emberCommon::ComputeShaderId ComputeShaderManager::AddComputeShader(const std::string& name, const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader)
	{
		if (pComputeShader == nullptr)
			throw std::runtime_error("ComputeShaderManager::AddComputeShader(...) failed. pComputeShader is nullptr.");
		return s_computeShaderSlotMap.Add(name, ManagedComputeShader(accessRights, std::move(pComputeShader)));
	}
	void ComputeShaderManager::RetireComputeShader(std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader)
	{
		if (pComputeShader != nullptr)
			GpuResourceFactory::RetireComputeShader(pComputeShader.release());
	}
	emberCommon::ComputeShaderId ComputeShaderManager::FindComputeShaderId(const std::string& name)
	{
		return s_computeShaderSlotMap.Find(name);
	}
}