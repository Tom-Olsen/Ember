#include "vulkanMaterialShaderManager.h"
#include "logger.h"
#include "vulkanGarbageCollector.h"
#include "vulkanMaterialManager.h"
#include "vulkanMaterialShader.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	MaterialShaderManager::MaterialShaderManager(uint32_t shadowMapResolution)
		: m_shadowMapResolution(shadowMapResolution)
		, m_pMaterialManager(nullptr)
	{

	}
	MaterialShaderManager::~MaterialShaderManager()
	{
		Clear();
	}



	// Creators:
	emberCommon::MaterialShaderId MaterialShaderManager::CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		emberCommon::MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (emberBackendInterface::IMaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
				throw std::runtime_error("MaterialShaderManager::CreateGizmoMaterialShader(...) failed. Existing MaterialShader is not a gizmo shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, true, std::make_unique<MaterialShader>(MaterialShader::CreateGizmo(vertexSpv, fragmentSpv, name)));
	}
	emberCommon::MaterialShaderId MaterialShaderManager::CreateOutlineMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		emberCommon::MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (emberBackendInterface::IMaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::outline)
				throw std::runtime_error("MaterialShaderManager::CreateOutlineMaterialShader(...) failed. Existing MaterialShader is not an outline shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, false, std::make_unique<MaterialShader>(MaterialShader::CreateOutline(vertexSpv, fragmentSpv, name)));
	}
	emberCommon::MaterialShaderId MaterialShaderManager::CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		emberCommon::MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (emberBackendInterface::IMaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::shadow)
				throw std::runtime_error("MaterialShaderManager::CreateShadowMaterialShader(...) failed. Existing MaterialShader is not a shadow shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, true, std::make_unique<MaterialShader>(MaterialShader::CreateShadow(m_shadowMapResolution, vertexSpv, name)));
	}
	emberCommon::MaterialShaderId MaterialShaderManager::CreateDeferredGeometryMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		emberCommon::MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (emberBackendInterface::IMaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
				throw std::runtime_error("MaterialShaderManager::CreateDeferredGeometryMaterialShader(...) failed. Existing MaterialShader is not a deferred geometry shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, true, std::make_unique<MaterialShader>(MaterialShader::CreateDeferredGeometry(vertexSpv, fragmentSpv, name)));
	}
	emberCommon::MaterialShaderId MaterialShaderManager::CreateDeferredLightingMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		emberCommon::MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (emberBackendInterface::IMaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::deferredLighting)
				throw std::runtime_error("MaterialShaderManager::CreateDeferredLightingMaterialShader(...) failed. Existing MaterialShader is not a deferred lighting shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, false, std::make_unique<MaterialShader>(MaterialShader::CreateDeferredLighting(vertexSpv, fragmentSpv, name)));
	}
	emberCommon::MaterialShaderId MaterialShaderManager::CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		emberCommon::MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (emberBackendInterface::IMaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::forward)
				throw std::runtime_error("MaterialShaderManager::CreateForwardMaterialShader(...) failed. Existing MaterialShader is not a forward shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, true, std::make_unique<MaterialShader>(MaterialShader::CreateForward(vertexSpv, fragmentSpv, name)));
	}
	emberCommon::MaterialShaderId MaterialShaderManager::CreatePresentMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		emberCommon::MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (emberBackendInterface::IMaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::present)
				throw std::runtime_error("MaterialShaderManager::CreatePresentMaterialShader(...) failed. Existing MaterialShader is not a present shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, false, std::make_unique<MaterialShader>(MaterialShader::CreatePresent(vertexSpv, fragmentSpv, name)));
	}



	// Getters:
	emberCommon::MaterialShaderId MaterialShaderManager::TryGetMaterialShaderId(const std::string& name) const
	{
		emberCommon::MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (TryGetMaterialShader(materialShaderId) == nullptr || !m_materialShaderSlots[materialShaderId.index].managedMaterialShader.isAccessible)
			return emberCommon::invalidMaterialShaderId;
		return materialShaderId;
	}
	emberBackendInterface::IMaterialShader* MaterialShaderManager::TryGetMaterialShader(emberCommon::MaterialShaderId materialShaderId) const
	{
		if (materialShaderId.index == emberCommon::invalidMaterialShaderId.index || materialShaderId.index >= m_materialShaderSlots.size())
			return nullptr;

		const MaterialShaderSlot& slot = m_materialShaderSlots[materialShaderId.index];
		if (slot.generation != materialShaderId.generation)
			return nullptr;
		return slot.managedMaterialShader.pMaterialShader.get();
	}
	const std::string* MaterialShaderManager::TryGetMaterialShaderName(emberCommon::MaterialShaderId materialShaderId) const
	{
		if (TryGetMaterialShader(materialShaderId) == nullptr)
			return nullptr;
		return &m_materialShaderSlots[materialShaderId.index].managedMaterialShader.name;
	}



	// Deleter:
	void MaterialShaderManager::DeleteMaterialShader(emberCommon::MaterialShaderId materialShaderId)
	{
		if (TryGetMaterialShader(materialShaderId) == nullptr)
			return;

		MaterialShaderSlot& slot = m_materialShaderSlots[materialShaderId.index];
		if (!slot.managedMaterialShader.isAccessible)
		{
			LOG_WARN("MaterialShaderManager::DeleteMaterialShader(...) failed. MaterialShader {} is internal and pinned until shutdown.", slot.managedMaterialShader.name);
			return;
		}
		if (m_pMaterialManager != nullptr && m_pMaterialManager->IsMaterialShaderInUse(materialShaderId))
			throw std::runtime_error("MaterialShaderManager::DeleteMaterialShader(...) failed. MaterialShader is still used by at least one material: " + slot.managedMaterialShader.name);

		m_materialShaderIdsMap.erase(slot.managedMaterialShader.name);
		DestroyMaterialShader(std::move(slot.managedMaterialShader.pMaterialShader));
		slot.managedMaterialShader.name.clear();
		slot.managedMaterialShader.isAccessible = false;
		slot.generation++;
		m_freeMaterialShaderIds.push_back(materialShaderId.index);
	}



	// Debugging:
	void MaterialShaderManager::Print() const
	{
		LOG_TRACE("MaterialShaderManager contents:");
		for (const auto& [name, index] : m_materialShaderIdsMap)
			LOG_TRACE("  {}: index {}, generation {}, accessible {}", name, index, m_materialShaderSlots[index].generation, m_materialShaderSlots[index].managedMaterialShader.isAccessible);
	}



	// Private methods:
	// Management:
	emberCommon::MaterialShaderId MaterialShaderManager::AddMaterialShader(const std::string& name, bool isAccessible, std::unique_ptr<MaterialShader> pMaterialShader)
	{
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialShaderManager::AddMaterialShader(...) failed. pMaterialShader is nullptr.");

		emberCommon::MaterialShaderId materialShaderId;
		if (m_freeMaterialShaderIds.empty())
		{
			if (m_materialShaderSlots.size() >= emberCommon::invalidMaterialShaderId.index)
				throw std::runtime_error("MaterialShaderManager::AddMaterialShader(...) failed. MaterialShader id limit reached.");
			materialShaderId.index = static_cast<uint32_t>(m_materialShaderSlots.size());
			m_materialShaderSlots.push_back({ 1, ManagedMaterialShader{ name, isAccessible, std::move(pMaterialShader) } });
		}
		else
		{
			materialShaderId.index = m_freeMaterialShaderIds.back();
			m_freeMaterialShaderIds.pop_back();

			MaterialShaderSlot& slot = m_materialShaderSlots[materialShaderId.index];
			slot.managedMaterialShader.name = name;
			slot.managedMaterialShader.isAccessible = isAccessible;
			slot.managedMaterialShader.pMaterialShader = std::move(pMaterialShader);
		}

		materialShaderId.generation = m_materialShaderSlots[materialShaderId.index].generation;
		m_materialShaderIdsMap[name] = materialShaderId.index;
		return materialShaderId;
	}
	void MaterialShaderManager::DestroyMaterialShader(std::unique_ptr<MaterialShader> pMaterialShader)
	{
		MaterialShader* pMaterialShaderRaw = pMaterialShader.release();
		if (pMaterialShaderRaw == nullptr)
			return;
		GarbageCollector::RecordFrameGarbage([pMaterialShaderRaw]()
		{
			delete pMaterialShaderRaw;
		});
	}
	void MaterialShaderManager::Clear()
	{
		for (uint32_t index = 0; index < m_materialShaderSlots.size(); index++)
		{
			const MaterialShaderSlot& slot = m_materialShaderSlots[index];
			if (slot.managedMaterialShader.pMaterialShader != nullptr && m_pMaterialManager != nullptr && m_pMaterialManager->IsMaterialShaderInUse({ index, slot.generation }))
				throw std::runtime_error("MaterialShaderManager::Clear() failed. MaterialShader is still used by at least one material: " + slot.managedMaterialShader.name);
		}

		for (uint32_t index = 0; index < m_materialShaderSlots.size(); index++)
		{
			MaterialShaderSlot& slot = m_materialShaderSlots[index];
			if (slot.managedMaterialShader.pMaterialShader == nullptr)
				continue;

			DestroyMaterialShader(std::move(slot.managedMaterialShader.pMaterialShader));
			slot.managedMaterialShader.name.clear();
			slot.managedMaterialShader.isAccessible = false;
			slot.generation++;
			m_freeMaterialShaderIds.push_back(index);
		}
		m_materialShaderIdsMap.clear();
	}
	emberCommon::MaterialShaderId MaterialShaderManager::FindMaterialShaderId(const std::string& name) const
	{
		auto it = m_materialShaderIdsMap.find(name);
		if (it == m_materialShaderIdsMap.end())
			return emberCommon::invalidMaterialShaderId;

		const uint32_t index = it->second;
		return emberCommon::MaterialShaderId{ index, m_materialShaderSlots[index].generation };
	}
	void MaterialShaderManager::LinkMaterialManager(MaterialManager* pMaterialManager)
	{
		m_pMaterialManager = pMaterialManager;
	}
}