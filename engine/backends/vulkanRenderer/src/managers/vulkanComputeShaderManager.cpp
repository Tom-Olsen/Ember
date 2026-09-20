#include "vulkanComputeShaderManager.h"
#include "computeShaderAsset.h"
#include "logger.h"
#include "vulkanComputeShader.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanGarbageCollector.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Managed compute shader constructor:
	ComputeShaderManager::ManagedComputeShader::ManagedComputeShader(std::string name, const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<ComputeShader> pComputeShader)
		: name(std::move(name))
		, accessRights(accessRights)
		, pComputeShader(std::move(pComputeShader))
	{

	}



	// Compute shader slot constructor:
	ComputeShaderManager::ComputeShaderSlot::ComputeShaderSlot(uint32_t generation, ManagedComputeShader managedComputeShader)
		: generation(generation)
		, managedComputeShader(std::move(managedComputeShader))
	{

	}



	// Public methods:
	// Constructor/Destructor:
	ComputeShaderManager::ComputeShaderManager()
	{

	}
	ComputeShaderManager::~ComputeShaderManager()
	{
		Clear();
	}



	// Creation:
	void ComputeShaderManager::InitializeDefaultComputeShaders()
	{
		DefaultGpuResources::SetDefaultComputeShaders(
			GetComputeShader("gammaCorrection"),
			GetComputeShader("outlineComposite"),
			GetComputeShader("outlineHorizontalMaskExpansion"),
			GetComputeShader("outlineVerticalMaskExpansion"));
	}
	emberCommon::ComputeShaderId ComputeShaderManager::CreateComputeShader(const emberAssetLoader::ComputeShaderAsset& computeShaderAsset)
	{
		emberCommon::ComputeShaderId computeShaderId = FindComputeShaderId(computeShaderAsset.computeShaderName);
		if (TryGetComputeShader(computeShaderId) != nullptr)
			return computeShaderId;

		return AddComputeShader(
			computeShaderAsset.computeShaderName,
			computeShaderAsset.accessRights,
			std::unique_ptr<ComputeShader>(new ComputeShader(computeShaderAsset.binaryPath, computeShaderAsset.features, computeShaderAsset.computeShaderName)));
	}



	// Getters:
	emberCommon::ComputeShaderId ComputeShaderManager::TryGetComputeShaderId(const std::string& name) const
	{
		emberCommon::ComputeShaderId computeShaderId = FindComputeShaderId(name);
		if (TryGetComputeShader(computeShaderId) == nullptr || !m_computeShaderSlots[computeShaderId.index].managedComputeShader.accessRights.isAccessible)
			return emberCommon::invalidComputeShaderId;
		return computeShaderId;
	}
	emberBackendInterface::IComputeShader* ComputeShaderManager::TryGetComputeShader(emberCommon::ComputeShaderId computeShaderId) const
	{
		if (computeShaderId.index == emberCommon::invalidComputeShaderId.index || computeShaderId.index >= m_computeShaderSlots.size())
			return nullptr;

		const ComputeShaderSlot& slot = m_computeShaderSlots[computeShaderId.index];
		if (slot.generation != computeShaderId.generation)
			return nullptr;
		return slot.managedComputeShader.pComputeShader.get();
	}
	const std::string* ComputeShaderManager::TryGetComputeShaderName(emberCommon::ComputeShaderId computeShaderId) const
	{
		if (TryGetComputeShader(computeShaderId) == nullptr)
			return nullptr;
		return &m_computeShaderSlots[computeShaderId.index].managedComputeShader.name;
	}
	bool ComputeShaderManager::IsComputeShaderMutable(emberCommon::ComputeShaderId computeShaderId) const
	{
		return TryGetComputeShader(computeShaderId) != nullptr && m_computeShaderSlots[computeShaderId.index].managedComputeShader.accessRights.isMutable;
	}



	// Debugging:
	void ComputeShaderManager::Print() const
	{
		LOG_TRACE("ComputeShaderManager contents:");
		for (const auto& [name, index] : m_computeShaderIdsMap)
		{
			const ComputeShaderSlot& slot = m_computeShaderSlots[index];
			const emberCommon::ResourceAccessRights& accessRights = slot.managedComputeShader.accessRights;
			LOG_TRACE("  {}: index {}, generation {}, accessible {}, deletable {}, mutable {}", name, index, slot.generation, accessRights.isAccessible, accessRights.isDeletable, accessRights.isMutable);
		}
	}



	// Private methods:
	// Management:
	emberCommon::ComputeShaderId ComputeShaderManager::AddComputeShader(const std::string& name, const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<ComputeShader> pComputeShader)
	{
		if (pComputeShader == nullptr)
			throw std::runtime_error("ComputeShaderManager::AddComputeShader(...) failed. pComputeShader is nullptr.");
		if (TryGetComputeShader(FindComputeShaderId(name)) != nullptr)
			throw std::runtime_error("ComputeShaderManager::AddComputeShader(...) failed. ComputeShader already exists: " + name);

		emberCommon::ComputeShaderId computeShaderId;
		if (m_freeComputeShaderIds.empty())
		{
			if (m_computeShaderSlots.size() >= emberCommon::invalidComputeShaderId.index)
				throw std::runtime_error("ComputeShaderManager::AddComputeShader(...) failed. Compute shader id limit reached.");
			computeShaderId.index = static_cast<uint32_t>(m_computeShaderSlots.size());
			m_computeShaderSlots.emplace_back(1, ManagedComputeShader(name, accessRights, std::move(pComputeShader)));
		}
		else
		{
			computeShaderId.index = m_freeComputeShaderIds.back();
			m_freeComputeShaderIds.pop_back();

			ComputeShaderSlot& slot = m_computeShaderSlots[computeShaderId.index];
			slot.managedComputeShader.name = name;
			slot.managedComputeShader.accessRights = accessRights;
			slot.managedComputeShader.pComputeShader = std::move(pComputeShader);
		}

		computeShaderId.generation = m_computeShaderSlots[computeShaderId.index].generation;
		m_computeShaderIdsMap[name] = computeShaderId.index;
		return computeShaderId;
	}
	void ComputeShaderManager::Clear()
	{
		DefaultGpuResources::ClearDefaultComputeShaders();
		for (uint32_t index = 0; index < m_computeShaderSlots.size(); index++)
		{
			ComputeShaderSlot& slot = m_computeShaderSlots[index];
			if (slot.managedComputeShader.pComputeShader == nullptr)
				continue;

			DestroyComputeShader(std::move(slot.managedComputeShader.pComputeShader));
			slot.managedComputeShader.name.clear();
			slot.managedComputeShader.accessRights = { false, false, false };
			slot.generation++;
			m_freeComputeShaderIds.push_back(index);
		}
		m_computeShaderIdsMap.clear();
	}
	void ComputeShaderManager::DestroyComputeShader(std::unique_ptr<ComputeShader> pComputeShader)
	{
		ComputeShader* pComputeShaderRaw = pComputeShader.release();
		if (pComputeShaderRaw == nullptr)
			return;
		GarbageCollector::RecordPendingGarbage([pComputeShaderRaw]()
		{
			if (pComputeShaderRaw->HasPendingUse())
				return false;
			delete pComputeShaderRaw;
			return true;
		});
	}
	emberCommon::ComputeShaderId ComputeShaderManager::FindComputeShaderId(const std::string& name) const
	{
		auto iterator = m_computeShaderIdsMap.find(name);
		if (iterator == m_computeShaderIdsMap.end())
			return emberCommon::invalidComputeShaderId;

		const uint32_t index = iterator->second;
		return emberCommon::ComputeShaderId{ index, m_computeShaderSlots[index].generation };
	}
	ComputeShader* ComputeShaderManager::GetComputeShader(const std::string& name) const
	{
		ComputeShader* pComputeShader = static_cast<ComputeShader*>(TryGetComputeShader(FindComputeShaderId(name)));
		if (pComputeShader == nullptr)
			throw std::runtime_error("ComputeShaderManager::GetComputeShader(...) failed. ComputeShader not found: " + name);
		return pComputeShader;
	}
}