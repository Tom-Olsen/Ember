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
#include <stdexcept>
#include <utility>
#include <vector>



namespace emberCore
{
	// Static members:
	bool ComputeShaderManager::s_isInitialized = false;
	std::unordered_map<std::string, uint32_t> ComputeShaderManager::s_computeShaderIdsMap;
	std::vector<ComputeShaderManager::ComputeShaderSlot> ComputeShaderManager::s_computeShaderSlots;
	std::vector<uint32_t> ComputeShaderManager::s_freeComputeShaderIds;



	// Managed compute shader methods:
	ComputeShaderManager::ManagedComputeShader::ManagedComputeShader()
		: accessRights{ false, false, false }
	{

	}
	ComputeShaderManager::ManagedComputeShader::ManagedComputeShader(std::string name, const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader)
		: name(std::move(name))
		, accessRights(accessRights)
		, pComputeShader(std::move(pComputeShader))
	{

	}
	ComputeShaderManager::ManagedComputeShader::~ManagedComputeShader() = default;
	ComputeShaderManager::ManagedComputeShader::ManagedComputeShader(ManagedComputeShader&&) noexcept = default;
	ComputeShaderManager::ManagedComputeShader& ComputeShaderManager::ManagedComputeShader::operator=(ManagedComputeShader&&) noexcept = default;



	// Compute shader slot methods:
	ComputeShaderManager::ComputeShaderSlot::ComputeShaderSlot(uint32_t generation, ManagedComputeShader managedComputeShader)
		: generation(generation)
		, managedComputeShader(std::move(managedComputeShader))
	{

	}
	ComputeShaderManager::ComputeShaderSlot::~ComputeShaderSlot() = default;
	ComputeShaderManager::ComputeShaderSlot::ComputeShaderSlot(ComputeShaderSlot&&) noexcept = default;
	ComputeShaderManager::ComputeShaderSlot& ComputeShaderManager::ComputeShaderSlot::operator=(ComputeShaderSlot&&) noexcept = default;



	// Public methods:
	// Asset loading:
	void ComputeShaderManager::LoadComputeShaderAssets(const std::filesystem::path& directoryPath)
	{
		if (!GpuResourceFactory::s_isInitialized)
			throw std::runtime_error("ComputeShaderManager::LoadComputeShaderAssets(...) failed. Compute shader manager is not initialized.");
		if (!std::filesystem::is_directory(directoryPath))
			throw std::runtime_error("ComputeShaderManager::LoadComputeShaderAssets(...) failed. Directory does not exist: " + directoryPath.string());

		// Collect asset paths:
		std::vector<std::filesystem::path> assetPaths;
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_regular_file() && entry.path().filename().string().ends_with(".computeShaderAsset.json"))
				assetPaths.push_back(entry.path());
		}
		std::sort(assetPaths.begin(), assetPaths.end());

		// Load assets:
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
		for (const std::pair<const std::string, uint32_t>& computeShaderIdPair : s_computeShaderIdsMap)
		{
			const std::string& name = computeShaderIdPair.first;
			const uint32_t index = computeShaderIdPair.second;
			const ComputeShaderSlot& slot = s_computeShaderSlots[index];
			const emberCommon::ResourceAccessRights& accessRights = slot.managedComputeShader.accessRights;
			LOG_TRACE("  {}: index {}, generation {}, accessible {}, deletable {}, mutable {}", name, index, slot.generation, accessRights.isAccessible, accessRights.isDeletable, accessRights.isMutable);
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
		std::unique_ptr<emberBackendInterface::IComputeShader> pGammaCorrectionComputeShader = TakeComputeShaderInterface("gammaCorrection");
		std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineCompositeComputeShader = TakeComputeShaderInterface("outlineComposite");
		std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineHorizontalMaskExpansionComputeShader = TakeComputeShaderInterface("outlineHorizontalMaskExpansion");
		std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineVerticalMaskExpansionComputeShader = TakeComputeShaderInterface("outlineVerticalMaskExpansion");
		DefaultGpuResources::InitializeBuiltInComputeShaders(
			std::move(pGammaCorrectionComputeShader),
			std::move(pOutlineCompositeComputeShader),
			std::move(pOutlineHorizontalMaskExpansionComputeShader),
			std::move(pOutlineVerticalMaskExpansionComputeShader));
		s_isInitialized = true;
	}
	void ComputeShaderManager::Clear()
	{
		if (!s_isInitialized)
			return;

		for (uint32_t index = 0; index < s_computeShaderSlots.size(); index++)
		{
			ComputeShaderSlot& slot = s_computeShaderSlots[index];
			if (slot.managedComputeShader.pComputeShader == nullptr)
				continue;

			RetireComputeShader(std::move(slot.managedComputeShader.pComputeShader));
			InvalidateComputeShaderSlot(index);
		}
		s_computeShaderIdsMap.clear();
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
		emberCommon::ComputeShaderCreateInfo createInfo
		{
			computeShaderAsset.binaryPath,
			computeShaderAsset.features,
			computeShaderAsset.computeShaderName
		};
		std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader(GpuResourceFactory::CreateComputeShader(createInfo));
		if (pComputeShader == nullptr)
			throw std::runtime_error("ComputeShaderManager::CreateComputeShader(...) failed. Gpu resource factory returned nullptr for: " + computeShaderAsset.computeShaderName);

		return AddComputeShader(computeShaderAsset.computeShaderName, computeShaderAsset.accessRights, std::move(pComputeShader));
	}



	// Getters:
	emberCommon::ComputeShaderId ComputeShaderManager::TryGetAccessibleComputeShaderId(const std::string& name)
	{
		emberCommon::ComputeShaderId computeShaderId = FindComputeShaderId(name);
		if (TryGetComputeShaderInterface(computeShaderId) == nullptr || !s_computeShaderSlots[computeShaderId.index].managedComputeShader.accessRights.isAccessible)
			return emberCommon::invalidComputeShaderId;
		return computeShaderId;
	}
	bool ComputeShaderManager::IsComputeShaderMutable(emberCommon::ComputeShaderId computeShaderId)
	{
		return TryGetComputeShaderInterface(computeShaderId) != nullptr && s_computeShaderSlots[computeShaderId.index].managedComputeShader.accessRights.isMutable;
	}
	emberBackendInterface::IComputeShader* ComputeShaderManager::TryGetComputeShaderInterface(emberCommon::ComputeShaderId computeShaderId)
	{
		if (computeShaderId.index == emberCommon::invalidComputeShaderId.index || computeShaderId.index >= s_computeShaderSlots.size())
			return nullptr;

		const ComputeShaderSlot& slot = s_computeShaderSlots[computeShaderId.index];
		if (slot.generation != computeShaderId.generation)
			return nullptr;
		return slot.managedComputeShader.pComputeShader.get();
	}
	const std::string* ComputeShaderManager::TryGetComputeShaderName(emberCommon::ComputeShaderId computeShaderId)
	{
		if (TryGetComputeShaderInterface(computeShaderId) == nullptr)
			return nullptr;
		return &s_computeShaderSlots[computeShaderId.index].managedComputeShader.name;
	}



	// Ownership transfer:
	std::unique_ptr<emberBackendInterface::IComputeShader> ComputeShaderManager::TakeComputeShaderInterface(const std::string& name)
	{
		emberCommon::ComputeShaderId computeShaderId = FindComputeShaderId(name);
		if (TryGetComputeShaderInterface(computeShaderId) == nullptr)
			throw std::runtime_error("ComputeShaderManager::TakeComputeShaderInterface(...) failed. ComputeShader not found: " + name);

		ComputeShaderSlot& slot = s_computeShaderSlots[computeShaderId.index];
		s_computeShaderIdsMap.erase(slot.managedComputeShader.name);
		std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader = std::move(slot.managedComputeShader.pComputeShader);
		InvalidateComputeShaderSlot(computeShaderId.index);
		return pComputeShader;
	}



	// Deleter:
	void ComputeShaderManager::DeleteComputeShader(emberCommon::ComputeShaderId computeShaderId)
	{
		if (TryGetComputeShaderInterface(computeShaderId) == nullptr)
			return;

		ComputeShaderSlot& slot = s_computeShaderSlots[computeShaderId.index];
		if (!slot.managedComputeShader.accessRights.isDeletable)
		{
			LOG_WARN("ComputeShaderManager::DeleteComputeShader(...) failed. ComputeShader '{}' is not deletable.", slot.managedComputeShader.name);
			return;
		}

		s_computeShaderIdsMap.erase(slot.managedComputeShader.name);
		RetireComputeShader(std::move(slot.managedComputeShader.pComputeShader));
		InvalidateComputeShaderSlot(computeShaderId.index);
	}



	// Management:
	emberCommon::ComputeShaderId ComputeShaderManager::AddComputeShader(const std::string& name, const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader)
	{
		if (pComputeShader == nullptr)
			throw std::runtime_error("ComputeShaderManager::AddComputeShader(...) failed. pComputeShader is nullptr.");
		if (TryGetComputeShaderInterface(FindComputeShaderId(name)) != nullptr)
			throw std::runtime_error("ComputeShaderManager::AddComputeShader(...) failed. ComputeShader already exists: " + name);

		emberCommon::ComputeShaderId computeShaderId;
		if (s_freeComputeShaderIds.empty())
		{
			if (s_computeShaderSlots.size() >= emberCommon::invalidComputeShaderId.index)
				throw std::runtime_error("ComputeShaderManager::AddComputeShader(...) failed. Compute shader id limit reached.");
			computeShaderId.index = static_cast<uint32_t>(s_computeShaderSlots.size());
			s_computeShaderSlots.emplace_back(1, ManagedComputeShader(name, accessRights, std::move(pComputeShader)));
		}
		else
		{
			computeShaderId.index = s_freeComputeShaderIds.back();
			s_freeComputeShaderIds.pop_back();

			ComputeShaderSlot& slot = s_computeShaderSlots[computeShaderId.index];
			slot.managedComputeShader.name = name;
			slot.managedComputeShader.accessRights = accessRights;
			slot.managedComputeShader.pComputeShader = std::move(pComputeShader);
		}

		computeShaderId.generation = s_computeShaderSlots[computeShaderId.index].generation;
		s_computeShaderIdsMap[name] = computeShaderId.index;
		return computeShaderId;
	}
	void ComputeShaderManager::RetireComputeShader(std::unique_ptr<emberBackendInterface::IComputeShader> pComputeShader)
	{
		if (pComputeShader != nullptr)
			GpuResourceFactory::RetireComputeShader(pComputeShader.release());
	}
	emberCommon::ComputeShaderId ComputeShaderManager::FindComputeShaderId(const std::string& name)
	{
		std::unordered_map<std::string, uint32_t>::const_iterator iterator = s_computeShaderIdsMap.find(name);
		if (iterator == s_computeShaderIdsMap.end())
			return emberCommon::invalidComputeShaderId;

		const uint32_t index = iterator->second;
		return emberCommon::ComputeShaderId{ index, s_computeShaderSlots[index].generation };
	}
	void ComputeShaderManager::InvalidateComputeShaderSlot(uint32_t index)
	{
		ComputeShaderSlot& slot = s_computeShaderSlots[index];
		slot.managedComputeShader.name.clear();
		slot.managedComputeShader.accessRights = { false, false, false };
		slot.generation++;
		if (slot.generation != emberCommon::invalidComputeShaderId.generation)
			s_freeComputeShaderIds.push_back(index);
	}
}