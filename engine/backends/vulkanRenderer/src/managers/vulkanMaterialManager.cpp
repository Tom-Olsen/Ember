#include "vulkanMaterialManager.h"
#include "iMaterialShader.h"
#include "logger.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanGarbageCollector.h"
#include "vulkanMaterial.h"
#include "vulkanMaterialShader.h"
#include "vulkanMaterialShaderManager.h"
#include <filesystem>
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	MaterialManager::MaterialManager(MaterialShaderManager* pMaterialShaderManager)
		: m_pMaterialShaderManager(pMaterialShaderManager)
		, m_defaultShadowMaterialId(emberCommon::invalidMaterialId)
	{
		// Link material shader manager:
		if (m_pMaterialShaderManager == nullptr)
			throw std::runtime_error("MaterialManager::MaterialManager(...) failed. pMaterialShaderManager is nullptr.");
		m_pMaterialShaderManager->LinkMaterialManager(this);

		// Create material shaders:
		const std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
		emberCommon::MaterialShaderId outlineShaderId = m_pMaterialShaderManager->CreateOutlineMaterialShader(directoryPath / "outline.vert.spv", directoryPath / "outline.frag.spv", "outlineShader");
		emberCommon::MaterialShaderId shadowShaderId = m_pMaterialShaderManager->CreateShadowMaterialShader(directoryPath / "shadow.vert.spv", "shadowShader");
		emberCommon::MaterialShaderId deferredLightingShaderId = m_pMaterialShaderManager->CreateDeferredLightingMaterialShader(directoryPath / "deferredLighting.vert.spv", directoryPath / "deferredLighting.frag.spv", "deferredLightingShader");
		emberCommon::MaterialShaderId presentShaderId = m_pMaterialShaderManager->CreatePresentMaterialShader(directoryPath / "present.vert.spv", directoryPath / "present.frag.spv", "presentShader");

		// Create default materials:
		emberCommon::MaterialId outlineMaterialId = CreateOutlineMaterial(outlineShaderId, "outlineMaterial");
		m_defaultShadowMaterialId = CreateShadowMaterial(shadowShaderId, "defaultShadowMaterial");
		SetAccessRights(m_defaultShadowMaterialId, true, false, true);
		emberCommon::MaterialId deferredLightingMaterialId = CreateDeferredLightingMaterial(deferredLightingShaderId, "deferredLightingMaterial");
		emberCommon::MaterialId presentMaterialId = CreatePresentMaterial(presentShaderId, "presentMaterial");
		DefaultGpuResources::SetDefaultMaterials(
			TryGetMaterial(outlineMaterialId),
			TryGetMaterial(m_defaultShadowMaterialId),
			TryGetMaterial(deferredLightingMaterialId),
			TryGetMaterial(presentMaterialId));
	}
	MaterialManager::~MaterialManager()
	{
		Clear();
		if (m_pMaterialShaderManager != nullptr)
			m_pMaterialShaderManager->LinkMaterialManager(nullptr);
	}



	// Creators:
	emberCommon::MaterialId MaterialManager::CreateGizmoMaterial(emberCommon::MaterialShaderId materialShaderId, emberCommon::GizmoRenderMode renderMode, const std::string& name)
	{
		MaterialShader* pMaterialShader = static_cast<MaterialShader*>(m_pMaterialShaderManager->TryGetMaterialShader(materialShaderId));
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateGizmoMaterial(...) failed. MaterialShader is invalid or expired.");
		if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("MaterialManager::CreateGizmoMaterial(...) failed. MaterialShader is not a gizmo shader.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}
		return AddMaterial(name, true, true, true, materialShaderId, std::make_unique<Material>(Material::CreateGizmo(pMaterialShader, renderMode, name)));
	}
	emberCommon::MaterialId MaterialManager::CreateOutlineMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name)
	{
		MaterialShader* pMaterialShader = static_cast<MaterialShader*>(m_pMaterialShaderManager->TryGetMaterialShader(materialShaderId));
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateOutlineMaterial(...) failed. MaterialShader is invalid or expired.");
		if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::outline)
			throw std::runtime_error("MaterialManager::CreateOutlineMaterial(...) failed. MaterialShader is not a outline shader.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}
		return AddMaterial(name, false, false, false, materialShaderId, std::make_unique<Material>(Material::CreateOutline(pMaterialShader, name)));
	}
	emberCommon::MaterialId MaterialManager::CreateShadowMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name)
	{
		MaterialShader* pMaterialShader = static_cast<MaterialShader*>(m_pMaterialShaderManager->TryGetMaterialShader(materialShaderId));
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateShadowMaterial(...) failed. MaterialShader is invalid or expired.");
		if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::CreateShadowMaterial(...) failed. MaterialShader is not a shadow shader.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}
		return AddMaterial(name, true, true, true, materialShaderId, std::make_unique<Material>(Material::CreateShadow(pMaterialShader, name)));
	}
	emberCommon::MaterialId MaterialManager::CreateDeferredGeometryMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name)
	{
		MaterialShader* pMaterialShader = static_cast<MaterialShader*>(m_pMaterialShaderManager->TryGetMaterialShader(materialShaderId));
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateDeferredGeometryMaterial(...) failed. MaterialShader is invalid or expired.");
		if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
			throw std::runtime_error("MaterialManager::CreateDeferredGeometryMaterial(...) failed. MaterialShader is not a deferred geometry shader.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}
		return AddMaterial(name, true, true, true, materialShaderId, std::make_unique<Material>(Material::CreateDeferredGeometry(pMaterialShader, name)));
	}
	emberCommon::MaterialId MaterialManager::CreateDeferredLightingMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name)
	{
		MaterialShader* pMaterialShader = static_cast<MaterialShader*>(m_pMaterialShaderManager->TryGetMaterialShader(materialShaderId));
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateDeferredLightingMaterial(...) failed. MaterialShader is invalid or expired.");
		if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::deferredLighting)
			throw std::runtime_error("MaterialManager::CreateDeferredLightingMaterial(...) failed. MaterialShader is not a deferred lighting shader.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}
		return AddMaterial(name, false, false, false, materialShaderId, std::make_unique<Material>(Material::CreateDeferredLighting(pMaterialShader, name)));
	}
	emberCommon::MaterialId MaterialManager::CreateForwardMaterial(emberCommon::MaterialShaderId materialShaderId, emberCommon::ForwardRenderMode renderMode, const std::string& name)
	{
		MaterialShader* pMaterialShader = static_cast<MaterialShader*>(m_pMaterialShaderManager->TryGetMaterialShader(materialShaderId));
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateForwardMaterial(...) failed. MaterialShader is invalid or expired.");
		if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("MaterialManager::CreateForwardMaterial(...) failed. MaterialShader is not a forward shader.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}
		return AddMaterial(name, true, true, true, materialShaderId, std::make_unique<Material>(Material::CreateForward(pMaterialShader, renderMode, name)));
	}
	emberCommon::MaterialId MaterialManager::CreatePresentMaterial(emberCommon::MaterialShaderId materialShaderId, const std::string& name)
	{
		MaterialShader* pMaterialShader = static_cast<MaterialShader*>(m_pMaterialShaderManager->TryGetMaterialShader(materialShaderId));
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreatePresentMaterial(...) failed. MaterialShader is invalid or expired.");
		if (pMaterialShader->GetMaterialPass() != emberCommon::MaterialPass::present)
			throw std::runtime_error("MaterialManager::CreatePresentMaterial(...) failed. MaterialShader is not a present shader.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}
		return AddMaterial(name, false, false, false, materialShaderId, std::make_unique<Material>(Material::CreatePresent(pMaterialShader, name)));
	}



	// Cloners:
	emberCommon::MaterialId MaterialManager::CloneGizmoMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name)
	{
		Material* pSourceMaterial = static_cast<Material*>(TryGetMaterial(sourceMaterialId));
		if (pSourceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material is invalid or expired.");
		if (pSourceMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material is not a gizmo material.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}

		const emberCommon::MaterialShaderId* pMaterialShaderId = TryGetMaterialShaderId(sourceMaterialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material shader is invalid.");
		return AddMaterial(name, true, true, true, *pMaterialShaderId, std::make_unique<Material>(Material::CloneGizmo(*pSourceMaterial, name)));
	}
	emberCommon::MaterialId MaterialManager::CloneGizmoMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::GizmoRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = CloneGizmoMaterial(sourceMaterialId, name);
		if (Material* pMaterial = static_cast<Material*>(TryGetMaterial(materialId)))
			pMaterial->SetGizmoRenderMode(renderMode);
		return materialId;
	}
	emberCommon::MaterialId MaterialManager::CloneOutlineMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name)
	{
		Material* pSourceMaterial = static_cast<Material*>(TryGetMaterial(sourceMaterialId));
		if (pSourceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CloneOutlineMaterial(...) failed. Source material is invalid or expired.");
		if (pSourceMaterial->GetMaterialPass() != emberCommon::MaterialPass::outline)
			throw std::runtime_error("MaterialManager::CloneOutlineMaterial(...) failed. Source material is not an outline material.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}

		const emberCommon::MaterialShaderId* pMaterialShaderId = TryGetMaterialShaderId(sourceMaterialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneOutlineMaterial(...) failed. Source material shader is invalid.");
		return AddMaterial(name, false, false, false, *pMaterialShaderId, std::make_unique<Material>(Material::CloneOutline(*pSourceMaterial, name)));
	}
	emberCommon::MaterialId MaterialManager::CloneShadowMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name)
	{
		Material* pSourceMaterial = static_cast<Material*>(TryGetMaterial(sourceMaterialId));
		if (pSourceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CloneShadowMaterial(...) failed. Source material is invalid or expired.");
		if (pSourceMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::CloneShadowMaterial(...) failed. Source material is not a shadow material.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}

		const emberCommon::MaterialShaderId* pMaterialShaderId = TryGetMaterialShaderId(sourceMaterialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneShadowMaterial(...) failed. Source material shader is invalid.");
		return AddMaterial(name, true, true, true, *pMaterialShaderId, std::make_unique<Material>(Material::CloneShadow(*pSourceMaterial, name)));
	}
	emberCommon::MaterialId MaterialManager::CloneDeferredGeometryMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name)
	{
		Material* pSourceMaterial = static_cast<Material*>(TryGetMaterial(sourceMaterialId));
		if (pSourceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CloneDeferredGeometryMaterial(...) failed. Source material is invalid or expired.");
		if (pSourceMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
			throw std::runtime_error("MaterialManager::CloneDeferredGeometryMaterial(...) failed. Source material is not a deferred geometry material.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}

		const emberCommon::MaterialShaderId* pMaterialShaderId = TryGetMaterialShaderId(sourceMaterialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneDeferredGeometryMaterial(...) failed. Source material shader is invalid.");
		emberCommon::MaterialId materialId = AddMaterial(name, true, true, true, *pMaterialShaderId, std::make_unique<Material>(Material::CloneDeferredGeometry(*pSourceMaterial, name)));
		emberCommon::MaterialId shadowMaterialId = TryGetShadowMaterialId(sourceMaterialId);
		if (shadowMaterialId.index != emberCommon::invalidMaterialId.index)
			SetShadowMaterial(materialId, shadowMaterialId);
		return materialId;
	}
	emberCommon::MaterialId MaterialManager::CloneDeferredLightingMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name)
	{
		Material* pSourceMaterial = static_cast<Material*>(TryGetMaterial(sourceMaterialId));
		if (pSourceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CloneDeferredLightingMaterial(...) failed. Source material is invalid or expired.");
		if (pSourceMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredLighting)
			throw std::runtime_error("MaterialManager::CloneDeferredLightingMaterial(...) failed. Source material is not a deferred lighting material.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}

		const emberCommon::MaterialShaderId* pMaterialShaderId = TryGetMaterialShaderId(sourceMaterialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneDeferredLightingMaterial(...) failed. Source material shader is invalid.");
		return AddMaterial(name, false, false, false, *pMaterialShaderId, std::make_unique<Material>(Material::CloneDeferredLighting(*pSourceMaterial, name)));
	}
	emberCommon::MaterialId MaterialManager::CloneForwardMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name)
	{
		Material* pSourceMaterial = static_cast<Material*>(TryGetMaterial(sourceMaterialId));
		if (pSourceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material is invalid or expired.");
		if (pSourceMaterial->GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material is not a forward material.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}

		const emberCommon::MaterialShaderId* pMaterialShaderId = TryGetMaterialShaderId(sourceMaterialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material shader is invalid.");
		emberCommon::MaterialId materialId = AddMaterial(name, true, true, true, *pMaterialShaderId, std::make_unique<Material>(Material::CloneForward(*pSourceMaterial, name)));
		emberCommon::MaterialId shadowMaterialId = TryGetShadowMaterialId(sourceMaterialId);
		if (shadowMaterialId.index != emberCommon::invalidMaterialId.index)
			SetShadowMaterial(materialId, shadowMaterialId);
		return materialId;
	}
	emberCommon::MaterialId MaterialManager::CloneForwardMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::ForwardRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = CloneForwardMaterial(sourceMaterialId, name);
		if (Material* pMaterial = static_cast<Material*>(TryGetMaterial(materialId)))
			pMaterial->SetForwardRenderMode(renderMode);
		return materialId;
	}
	emberCommon::MaterialId MaterialManager::ClonePresentMaterial(emberCommon::MaterialId sourceMaterialId, const std::string& name)
	{
		Material* pSourceMaterial = static_cast<Material*>(TryGetMaterial(sourceMaterialId));
		if (pSourceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::ClonePresentMaterial(...) failed. Source material is invalid or expired.");
		if (pSourceMaterial->GetMaterialPass() != emberCommon::MaterialPass::present)
			throw std::runtime_error("MaterialManager::ClonePresentMaterial(...) failed. Source material is not a present material.");
		if (TryGetMaterial(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}

		const emberCommon::MaterialShaderId* pMaterialShaderId = TryGetMaterialShaderId(sourceMaterialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::ClonePresentMaterial(...) failed. Source material shader is invalid.");
		return AddMaterial(name, false, false, false, *pMaterialShaderId, std::make_unique<Material>(Material::ClonePresent(*pSourceMaterial, name)));
	}


	// Getters:
	emberCommon::MaterialId MaterialManager::TryGetMaterialId(const std::string& name) const
	{
		emberCommon::MaterialId materialId = FindMaterialId(name);
		if (TryGetMaterial(materialId) == nullptr || !m_materialSlots[materialId.index].managedMaterial.isAccessible)
			return emberCommon::invalidMaterialId;
		return materialId;
	}
	emberCommon::MaterialId MaterialManager::GetDefaultShadowMaterialId() const
	{
		if (TryGetMaterial(m_defaultShadowMaterialId) == nullptr)
			throw std::runtime_error("MaterialManager::GetDefaultShadowMaterialId() failed. Default shadow material is not initialized.");
		return m_defaultShadowMaterialId;
	}
	emberCommon::MaterialId MaterialManager::TryGetShadowMaterialId(emberCommon::MaterialId surfaceMaterialId)
	{
		Material* pSurfaceMaterial = static_cast<Material*>(TryGetMaterial(surfaceMaterialId));
		if (pSurfaceMaterial == nullptr || !IsSurfaceMaterialPass(pSurfaceMaterial->GetMaterialPass()))
			return emberCommon::invalidMaterialId;

		emberCommon::MaterialId& shadowMaterialId = m_materialSlots[surfaceMaterialId.index].managedMaterial.shadowMaterialId;
		Material* pShadowMaterial = static_cast<Material*>(TryGetMaterial(shadowMaterialId));
		if (pShadowMaterial == nullptr || pShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			shadowMaterialId = m_defaultShadowMaterialId;

		pShadowMaterial = static_cast<Material*>(TryGetMaterial(shadowMaterialId));
		if (pShadowMaterial == nullptr || pShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			return emberCommon::invalidMaterialId;
		return shadowMaterialId;
	}
	emberBackendInterface::IMaterial* MaterialManager::TryGetMaterial(emberCommon::MaterialId materialId) const
	{
		if (materialId.index == emberCommon::invalidMaterialId.index || materialId.index >= m_materialSlots.size())
			return nullptr;

		const MaterialSlot& slot = m_materialSlots[materialId.index];
		if (slot.generation != materialId.generation)
			return nullptr;
		return slot.managedMaterial.pMaterial.get();
	}
	const std::string* MaterialManager::TryGetMaterialName(emberCommon::MaterialId materialId) const
	{
		if (TryGetMaterial(materialId) == nullptr)
			return nullptr;
		return &m_materialSlots[materialId.index].managedMaterial.name;
	}
	const emberCommon::MaterialShaderId* MaterialManager::TryGetMaterialShaderId(emberCommon::MaterialId materialId) const
	{
		if (TryGetMaterial(materialId) == nullptr)
			return nullptr;
		return &m_materialSlots[materialId.index].managedMaterial.materialShaderId;
	}
	bool MaterialManager::IsMaterialMutable(emberCommon::MaterialId materialId) const
	{
		return TryGetMaterial(materialId) != nullptr && m_materialSlots[materialId.index].managedMaterial.isMutable;
	}



	// Setters:
	void MaterialManager::SetAccessRights(emberCommon::MaterialId materialId, bool isAccessible, bool isDeletable, bool isMutable)
	{
		Material* pMaterial = static_cast<Material*>(TryGetMaterial(materialId));
		if (pMaterial == nullptr)
			throw std::runtime_error("MaterialManager::SetAccessRights(...) failed. Material is invalid or expired.");

		m_materialSlots[materialId.index].managedMaterial.isAccessible = isAccessible;
		m_materialSlots[materialId.index].managedMaterial.isDeletable = isDeletable;
		m_materialSlots[materialId.index].managedMaterial.isMutable = isMutable;
	}
	void MaterialManager::SetShadowMaterial(emberCommon::MaterialId surfaceMaterialId, emberCommon::MaterialId shadowMaterialId)
	{
		Material* pSurfaceMaterial = static_cast<Material*>(TryGetMaterial(surfaceMaterialId));
		if (pSurfaceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Surface material is invalid or expired.");
		if (!IsSurfaceMaterialPass(pSurfaceMaterial->GetMaterialPass()))
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Material is not a deferred or forward material.");

		Material* pShadowMaterial = static_cast<Material*>(TryGetMaterial(shadowMaterialId));
		if (pShadowMaterial == nullptr)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Shadow material is invalid or expired.");
		if (pShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Material is not a shadow material.");

		m_materialSlots[surfaceMaterialId.index].managedMaterial.shadowMaterialId = shadowMaterialId;
	}
	void MaterialManager::ResetShadowMaterial(emberCommon::MaterialId surfaceMaterialId)
	{
		Material* pSurfaceMaterial = static_cast<Material*>(TryGetMaterial(surfaceMaterialId));
		if (pSurfaceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::ResetShadowMaterial(...) failed. Surface material is invalid or expired.");
		if (!IsSurfaceMaterialPass(pSurfaceMaterial->GetMaterialPass()))
			throw std::runtime_error("MaterialManager::ResetShadowMaterial(...) failed. Material is not a deferred or forward material.");

		m_materialSlots[surfaceMaterialId.index].managedMaterial.shadowMaterialId = m_defaultShadowMaterialId;
	}



	// Deleter:
	void MaterialManager::DeleteMaterial(emberCommon::MaterialId materialId)
	{
		if (TryGetMaterial(materialId) == nullptr)
			return;
		MaterialSlot& slot = m_materialSlots[materialId.index];
		if (!slot.managedMaterial.isDeletable)
		{
			LOG_WARN("MaterialManager::DeleteMaterial(...) failed. Material {} is pinned until shutdown.", slot.managedMaterial.name);
			return;
		}

		m_materialIdsMap.erase(slot.managedMaterial.name);
		DestroyMaterial(std::move(slot.managedMaterial.pMaterial));
		slot.managedMaterial.name.clear();
		slot.managedMaterial.isAccessible = false;
		slot.managedMaterial.isDeletable = false;
		slot.managedMaterial.isMutable = false;
		slot.managedMaterial.materialShaderId = emberCommon::invalidMaterialShaderId;
		slot.managedMaterial.shadowMaterialId = emberCommon::invalidMaterialId;
		slot.generation++;
		m_freeMaterialIds.push_back(materialId.index);
	}



	// Debugging:
	void MaterialManager::Print() const
	{
		LOG_TRACE("MaterialManager contents:");
		for (const auto& [name, index] : m_materialIdsMap)
		{
			const MaterialSlot& slot = m_materialSlots[index];
			LOG_TRACE("  {}: index {}, generation {}, accessible {}, deletable {}, mutable {}", name, index, slot.generation, slot.managedMaterial.isAccessible, slot.managedMaterial.isDeletable, slot.managedMaterial.isMutable);
		}
	}



	// Private methods:
	// Management:
	emberCommon::MaterialId MaterialManager::AddMaterial(const std::string& name, bool isAccessible, bool isDeletable, bool isMutable, emberCommon::MaterialShaderId materialShaderId, std::unique_ptr<Material> pMaterial)
	{
		if (pMaterial == nullptr)
			throw std::runtime_error("MaterialManager::AddMaterial(...) failed. pMaterial is nullptr.");
		emberCommon::MaterialId shadowMaterialId = IsSurfaceMaterialPass(pMaterial->GetMaterialPass()) ? m_defaultShadowMaterialId : emberCommon::invalidMaterialId;

		emberCommon::MaterialId materialId;
		if (m_freeMaterialIds.empty())
		{
			if (m_materialSlots.size() >= emberCommon::invalidMaterialId.index)
				throw std::runtime_error("MaterialManager::AddMaterial(...) failed. Material id limit reached.");
			materialId.index = static_cast<uint32_t>(m_materialSlots.size());
			m_materialSlots.push_back({ 1, ManagedMaterial{ name, isAccessible, isDeletable, isMutable, materialShaderId, shadowMaterialId, std::move(pMaterial) } });
		}
		else
		{
			materialId.index = m_freeMaterialIds.back();
			m_freeMaterialIds.pop_back();

			MaterialSlot& slot = m_materialSlots[materialId.index];
			slot.managedMaterial.name = name;
			slot.managedMaterial.isAccessible = isAccessible;
			slot.managedMaterial.isDeletable = isDeletable;
			slot.managedMaterial.isMutable = isMutable;
			slot.managedMaterial.materialShaderId = materialShaderId;
			slot.managedMaterial.shadowMaterialId = shadowMaterialId;
			slot.managedMaterial.pMaterial = std::move(pMaterial);
		}

		materialId.generation = m_materialSlots[materialId.index].generation;
		m_materialIdsMap[name] = materialId.index;
		return materialId;
	}
	void MaterialManager::DestroyMaterial(std::unique_ptr<Material> pMaterial)
	{
		Material* pMaterialRaw = pMaterial.release();
		if (pMaterialRaw == nullptr)
			return;
		GarbageCollector::RecordFrameGarbage([pMaterialRaw]()
		{
			delete pMaterialRaw;
		});
	}
	void MaterialManager::Clear()
	{
		DefaultGpuResources::ClearDefaultMaterials();
		for (uint32_t index = 0; index < m_materialSlots.size(); index++)
		{
			MaterialSlot& slot = m_materialSlots[index];
			if (slot.managedMaterial.pMaterial == nullptr)
				continue;

			DestroyMaterial(std::move(slot.managedMaterial.pMaterial));
			slot.managedMaterial.name.clear();
			slot.managedMaterial.isAccessible = false;
			slot.managedMaterial.isDeletable = false;
			slot.managedMaterial.isMutable = false;
			slot.managedMaterial.materialShaderId = emberCommon::invalidMaterialShaderId;
			slot.managedMaterial.shadowMaterialId = emberCommon::invalidMaterialId;
			slot.generation++;
			m_freeMaterialIds.push_back(index);
		}
		m_materialIdsMap.clear();
		m_defaultShadowMaterialId = emberCommon::invalidMaterialId;
	}
	emberCommon::MaterialId MaterialManager::FindMaterialId(const std::string& name) const
	{
		auto it = m_materialIdsMap.find(name);
		if (it == m_materialIdsMap.end())
			return emberCommon::invalidMaterialId;

		const uint32_t index = it->second;
		return emberCommon::MaterialId{ index, m_materialSlots[index].generation };
	}
	bool MaterialManager::IsMaterialShaderInUse(emberCommon::MaterialShaderId materialShaderId) const
	{
		for (const MaterialSlot& slot : m_materialSlots)
			if (slot.managedMaterial.pMaterial != nullptr && slot.managedMaterial.materialShaderId.index == materialShaderId.index && slot.managedMaterial.materialShaderId.generation == materialShaderId.generation)
				return true;
		return false;
	}
	bool MaterialManager::IsSurfaceMaterialPass(emberCommon::MaterialPass materialPass) const
	{
		return materialPass == emberCommon::MaterialPass::deferredGeometry || materialPass == emberCommon::MaterialPass::forward;
	}
}