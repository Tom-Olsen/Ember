#include "vulkanMaterialShaderManager.h"
#include "logger.h"
#include "vulkanGarbageCollector.h"
#include "vulkanMaterialManager.h"
#include "vulkanMaterialShader.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Managed material shader constructor:
	MaterialShaderManager::ManagedMaterialShader::ManagedMaterialShader(std::string name, std::unique_ptr<MaterialShader> pMaterialShader)
		: name(std::move(name))
		, pMaterialShader(std::move(pMaterialShader))
	{

	}

	

	// Material shader slot constructor:
	MaterialShaderManager::MaterialShaderSlot::MaterialShaderSlot(uint32_t generation, ManagedMaterialShader managedMaterialShader)
		: generation(generation)
		, managedMaterialShader(std::move(managedMaterialShader))
	{

	}



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



	// Getters:
	MaterialShader* MaterialShaderManager::TryGetMaterialShader(MaterialShaderId materialShaderId) const
	{
		if (materialShaderId.index == invalidMaterialShaderId.index || materialShaderId.index >= m_materialShaderSlots.size())
			return nullptr;

		const MaterialShaderSlot& slot = m_materialShaderSlots[materialShaderId.index];
		if (slot.generation != materialShaderId.generation)
			return nullptr;
		return slot.managedMaterialShader.pMaterialShader.get();
	}



	// Deleter:
	void MaterialShaderManager::DeleteMaterialShader(MaterialShaderId materialShaderId)
	{
		if (TryGetMaterialShader(materialShaderId) == nullptr)
			return;

		MaterialShaderSlot& slot = m_materialShaderSlots[materialShaderId.index];
		if (m_pMaterialManager != nullptr && m_pMaterialManager->IsMaterialShaderInUse(materialShaderId))
			throw std::runtime_error("MaterialShaderManager::DeleteMaterialShader(...) failed. MaterialShader is still used by at least one material: " + slot.managedMaterialShader.name);

		m_materialShaderIdsMap.erase(slot.managedMaterialShader.name);
		DestroyMaterialShader(std::move(slot.managedMaterialShader.pMaterialShader));
		slot.managedMaterialShader.name.clear();
		slot.generation++;
		m_freeMaterialShaderIds.push_back(materialShaderId.index);
	}



	// Debugging:
	void MaterialShaderManager::Print() const
	{
		LOG_TRACE("MaterialShaderManager contents:");
		for (const auto& [name, index] : m_materialShaderIdsMap)
			LOG_TRACE("  {}: index {}, generation {}", name, index, m_materialShaderSlots[index].generation);
	}



	// Private methods:
	// Creators:
	MaterialShaderId MaterialShaderManager::CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (MaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
				throw std::runtime_error("MaterialShaderManager::CreateGizmoMaterialShader(...) failed. Existing MaterialShader is not a gizmo shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, std::make_unique<MaterialShader>(MaterialShader::CreateGizmoMaterialShader(vertexSpv, fragmentSpv, name)));
	}
	MaterialShaderId MaterialShaderManager::CreateOutlineMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (MaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::outline)
				throw std::runtime_error("MaterialShaderManager::CreateOutlineMaterialShader(...) failed. Existing MaterialShader is not an outline shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, std::make_unique<MaterialShader>(MaterialShader::CreateOutlineMaterialShader(vertexSpv, fragmentSpv, name)));
	}
	MaterialShaderId MaterialShaderManager::CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (MaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::shadow)
				throw std::runtime_error("MaterialShaderManager::CreateShadowMaterialShader(...) failed. Existing MaterialShader is not a shadow shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, std::make_unique<MaterialShader>(MaterialShader::CreateShadowMaterialShader(m_shadowMapResolution, vertexSpv, name)));
	}
	MaterialShaderId MaterialShaderManager::CreateDeferredGeometryMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (MaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
				throw std::runtime_error("MaterialShaderManager::CreateDeferredGeometryMaterialShader(...) failed. Existing MaterialShader is not a deferred geometry shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, std::make_unique<MaterialShader>(MaterialShader::CreateDeferredGeometryMaterialShader(vertexSpv, fragmentSpv, name)));
	}
	MaterialShaderId MaterialShaderManager::CreateDeferredLightingMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (MaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::deferredLighting)
				throw std::runtime_error("MaterialShaderManager::CreateDeferredLightingMaterialShader(...) failed. Existing MaterialShader is not a deferred lighting shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, std::make_unique<MaterialShader>(MaterialShader::CreateDeferredLightingMaterialShader(vertexSpv, fragmentSpv, name)));
	}
	MaterialShaderId MaterialShaderManager::CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (MaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::forward)
				throw std::runtime_error("MaterialShaderManager::CreateForwardMaterialShader(...) failed. Existing MaterialShader is not a forward shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, std::make_unique<MaterialShader>(MaterialShader::CreateForwardMaterialShader(vertexSpv, fragmentSpv, name)));
	}
	MaterialShaderId MaterialShaderManager::CreatePresentMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = FindMaterialShaderId(name);
		if (MaterialShader* pMaterialShader = TryGetMaterialShader(materialShaderId))
		{
			if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::present)
				throw std::runtime_error("MaterialShaderManager::CreatePresentMaterialShader(...) failed. Existing MaterialShader is not a present shader: " + name);
			return materialShaderId;
		}

		return AddMaterialShader(name, std::make_unique<MaterialShader>(MaterialShader::CreatePresentMaterialShader(vertexSpv, fragmentSpv, name)));
	}



	// Management:
	MaterialShaderId MaterialShaderManager::AddMaterialShader(const std::string& name, std::unique_ptr<MaterialShader> pMaterialShader)
	{
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialShaderManager::AddMaterialShader(...) failed. pMaterialShader is nullptr.");

		MaterialShaderId materialShaderId;
		if (m_freeMaterialShaderIds.empty())
		{
			if (m_materialShaderSlots.size() >= invalidMaterialShaderId.index)
				throw std::runtime_error("MaterialShaderManager::AddMaterialShader(...) failed. MaterialShader id limit reached.");
			materialShaderId.index = static_cast<uint32_t>(m_materialShaderSlots.size());
			m_materialShaderSlots.emplace_back(1, ManagedMaterialShader(name, std::move(pMaterialShader)));
		}
		else
		{
			materialShaderId.index = m_freeMaterialShaderIds.back();
			m_freeMaterialShaderIds.pop_back();

			MaterialShaderSlot& slot = m_materialShaderSlots[materialShaderId.index];
			slot.managedMaterialShader.name = name;
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
			slot.generation++;
			m_freeMaterialShaderIds.push_back(index);
		}
		m_materialShaderIdsMap.clear();
	}
	MaterialShaderId MaterialShaderManager::FindMaterialShaderId(const std::string& name) const
	{
		auto it = m_materialShaderIdsMap.find(name);
		if (it == m_materialShaderIdsMap.end())
			return invalidMaterialShaderId;

		const uint32_t index = it->second;
		return MaterialShaderId{ index, m_materialShaderSlots[index].generation };
	}
	void MaterialShaderManager::LinkMaterialManager(MaterialManager* pMaterialManager)
	{
		m_pMaterialManager = pMaterialManager;
	}
}