#include "materialManager.h"
#include "commonMaterialCreateInfo.h"
#include "defaultGpuResources.h"
#include "gpuResourceFactory.h"
#include "iMaterial.h"
#include "iMaterialShader.h"
#include "logger.h"
#include "materialAsset.h"
#include "materialAssetLoader.h"
#include "materialShader.h"
#include "materialShaderManager.h"
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <utility>
#include <vector>



namespace emberCore
{
	// Static members:
	bool MaterialManager::s_isInitialized = false;
	emberBackendInterface::IMaterial* MaterialManager::s_pIErrorMaterial = nullptr;
	emberBackendInterface::IMaterial* MaterialManager::s_pIErrorGizmoMaterial = nullptr;
	emberCommon::MaterialId MaterialManager::s_defaultShadowMaterialId = emberCommon::invalidMaterialId;
	std::unordered_map<std::string, uint32_t> MaterialManager::s_materialIdsMap;
	std::vector<MaterialManager::MaterialSlot> MaterialManager::s_materialSlots;
	std::vector<uint32_t> MaterialManager::s_freeMaterialIds;



	// Managed material methods:
	MaterialManager::ManagedMaterial::ManagedMaterial()
		: accessRights{ false, false, false }
		, materialShaderId(emberCommon::invalidMaterialShaderId)
		, shadowMaterialId(emberCommon::invalidMaterialId)
		, pMaterial(nullptr)
	{

	}
	MaterialManager::ManagedMaterial::ManagedMaterial(std::string name, const emberCommon::ResourceAccessRights& accessRights, emberCommon::MaterialShaderId materialShaderId, emberCommon::MaterialId shadowMaterialId, std::unique_ptr<emberBackendInterface::IMaterial> pMaterial)
		: name(std::move(name))
		, accessRights(accessRights)
		, materialShaderId(materialShaderId)
		, shadowMaterialId(shadowMaterialId)
		, pOwnedMaterial(std::move(pMaterial))
		, pMaterial(pOwnedMaterial.get())
	{

	}
	MaterialManager::ManagedMaterial::~ManagedMaterial() = default;
	MaterialManager::ManagedMaterial::ManagedMaterial(ManagedMaterial&&) noexcept = default;
	MaterialManager::ManagedMaterial& MaterialManager::ManagedMaterial::operator=(ManagedMaterial&&) noexcept = default;



	// Material slot methods:
	MaterialManager::MaterialSlot::MaterialSlot(uint32_t generation, ManagedMaterial managedMaterial)
		: generation(generation)
		, managedMaterial(std::move(managedMaterial))
	{

	}
	MaterialManager::MaterialSlot::~MaterialSlot() = default;
	MaterialManager::MaterialSlot::MaterialSlot(MaterialSlot&&) noexcept = default;
	MaterialManager::MaterialSlot& MaterialManager::MaterialSlot::operator=(MaterialSlot&&) noexcept = default;



	// Public methods:
	// Asset loading:
	void MaterialManager::LoadMaterialAssets(const std::filesystem::path& directoryPath)
	{
		// Error handling:
		if (!GpuResourceFactory::s_isInitialized)
			throw std::runtime_error("MaterialManager::LoadMaterialAssets(...) failed. Material manager is not initialized.");
		if (!std::filesystem::is_directory(directoryPath))
			throw std::runtime_error("MaterialManager::LoadMaterialAssets(...) failed. Directory does not exist: " + directoryPath.string());

		// Collect asset paths:
		std::vector<std::filesystem::path> assetPaths;
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_regular_file() && entry.path().filename().string().ends_with(".materialAsset.json"))
				assetPaths.push_back(entry.path());
		}
		std::sort(assetPaths.begin(), assetPaths.end());

		// Load material assets:
		std::vector<emberAssetLoader::MaterialAsset> materialAssets;
		materialAssets.reserve(assetPaths.size());
		for (const std::filesystem::path& assetPath : assetPaths)
			materialAssets.push_back(emberAssetLoader::MaterialAssetLoader::Load(assetPath));

		// Create materials:
		for (const emberAssetLoader::MaterialAsset& materialAsset : materialAssets)
			CreateMaterial(materialAsset);

		// Assign default shadow material to all surface materials:
		if (TryGetMaterialInterface(s_defaultShadowMaterialId) == nullptr)
			s_defaultShadowMaterialId = FindMaterialId("defaultShadowMaterial");
		if (TryGetMaterialInterface(s_defaultShadowMaterialId) != nullptr)
		{
			for (MaterialSlot& slot : s_materialSlots)
			{
				if (slot.managedMaterial.pMaterial != nullptr && emberCommon::IsSurfaceMaterialPass(slot.managedMaterial.pMaterial->GetMaterialPass()) && slot.managedMaterial.shadowMaterialId == emberCommon::invalidMaterialId)
					slot.managedMaterial.shadowMaterialId = s_defaultShadowMaterialId;
			}
		}
	}



	// Cloners:
	GizmoMaterial MaterialManager::CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, const std::string& name)
	{
		return CloneGizmoMaterial(sourceMaterial, sourceMaterial.GetRenderMode(), name);
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = CloneMaterial(sourceMaterial.m_materialId, emberCommon::MaterialPass::gizmo, emberCommon::MaterialBindingCloneMode::copyBindings, name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr)
			return GizmoMaterial();
		pMaterial->SetGizmoRenderMode(renderMode);
		return GizmoMaterial(materialId);
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterialWithDefaultBindings(const GizmoMaterial& sourceMaterial, const std::string& name)
	{
		return CloneGizmoMaterialWithDefaultBindings(sourceMaterial, sourceMaterial.GetRenderMode(), name);
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterialWithDefaultBindings(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = CloneMaterial(sourceMaterial.m_materialId, emberCommon::MaterialPass::gizmo, emberCommon::MaterialBindingCloneMode::defaultBindings, name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr)
			return GizmoMaterial();
		pMaterial->SetGizmoRenderMode(renderMode);
		return GizmoMaterial(materialId);
	}
	ShadowMaterial MaterialManager::CloneShadowMaterial(const ShadowMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = CloneMaterial(sourceMaterial.m_materialId, emberCommon::MaterialPass::shadow, emberCommon::MaterialBindingCloneMode::copyBindings, name);
		return materialId == emberCommon::invalidMaterialId ? ShadowMaterial() : ShadowMaterial(materialId);
	}
	DeferredMaterial MaterialManager::CloneDeferredGeometryMaterial(const DeferredMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = CloneMaterial(sourceMaterial.m_materialId, emberCommon::MaterialPass::deferredGeometry, emberCommon::MaterialBindingCloneMode::copyBindings, name);
		if (materialId != emberCommon::invalidMaterialId)
		{
			emberCommon::MaterialId shadowMaterialId = TryGetShadowMaterialIdOfSurfaceMaterial(sourceMaterial.m_materialId);
			if (shadowMaterialId != emberCommon::invalidMaterialId)
				SetShadowMaterial(materialId, shadowMaterialId);
		}
		return materialId == emberCommon::invalidMaterialId ? DeferredMaterial() : DeferredMaterial(materialId);
	}
	DeferredMaterial MaterialManager::CloneDeferredGeometryMaterialWithDefaultBindings(const DeferredMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = CloneMaterial(sourceMaterial.m_materialId, emberCommon::MaterialPass::deferredGeometry, emberCommon::MaterialBindingCloneMode::defaultBindings, name);
		if (materialId != emberCommon::invalidMaterialId)
		{
			emberCommon::MaterialId shadowMaterialId = TryGetShadowMaterialIdOfSurfaceMaterial(sourceMaterial.m_materialId);
			if (shadowMaterialId != emberCommon::invalidMaterialId)
				SetShadowMaterial(materialId, shadowMaterialId);
		}
		return materialId == emberCommon::invalidMaterialId ? DeferredMaterial() : DeferredMaterial(materialId);
	}
	ForwardMaterial MaterialManager::CloneForwardMaterial(const ForwardMaterial& sourceMaterial, const std::string& name)
	{
		return CloneForwardMaterial(sourceMaterial, sourceMaterial.GetRenderMode(), name);
	}
	ForwardMaterial MaterialManager::CloneForwardMaterial(const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = CloneMaterial(sourceMaterial.m_materialId, emberCommon::MaterialPass::forward, emberCommon::MaterialBindingCloneMode::copyBindings, name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr)
			return ForwardMaterial();
		pMaterial->SetForwardRenderMode(renderMode);
		emberCommon::MaterialId shadowMaterialId = TryGetShadowMaterialIdOfSurfaceMaterial(sourceMaterial.m_materialId);
		if (shadowMaterialId != emberCommon::invalidMaterialId)
			SetShadowMaterial(materialId, shadowMaterialId);
		return ForwardMaterial(materialId);
	}
	ForwardMaterial MaterialManager::CloneForwardMaterialWithDefaultBindings(const ForwardMaterial& sourceMaterial, const std::string& name)
	{
		return CloneForwardMaterialWithDefaultBindings(sourceMaterial, sourceMaterial.GetRenderMode(), name);
	}
	ForwardMaterial MaterialManager::CloneForwardMaterialWithDefaultBindings(const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = CloneMaterial(sourceMaterial.m_materialId, emberCommon::MaterialPass::forward, emberCommon::MaterialBindingCloneMode::defaultBindings, name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr)
			return ForwardMaterial();
		pMaterial->SetForwardRenderMode(renderMode);
		emberCommon::MaterialId shadowMaterialId = TryGetShadowMaterialIdOfSurfaceMaterial(sourceMaterial.m_materialId);
		if (shadowMaterialId != emberCommon::invalidMaterialId)
			SetShadowMaterial(materialId, shadowMaterialId);
		return ForwardMaterial(materialId);
	}



	// Getters:
	Material MaterialManager::TryGetMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		if (TryGetMaterialInterface(materialId) == nullptr)
		{
			LOG_WARN("MaterialManager::TryGetMaterial(...) failed. Material '{}' not found or inaccessible.", name);
			return GetMaterial(FindMaterialId("errorMaterial"));
		}
		return Material(materialId);
	}
	GizmoMaterial MaterialManager::TryGetGizmoMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
		{
			LOG_WARN("MaterialManager::TryGetGizmoMaterial(...) failed. Material '{}' not found, inaccessible, or not a gizmo material.", name);
			return GizmoMaterial();
		}
		return GizmoMaterial(materialId);
	}
	ShadowMaterial MaterialManager::TryGetShadowMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
		{
			LOG_WARN("MaterialManager::TryGetShadowMaterial(...) failed. Material '{}' not found, inaccessible, or not a shadow material.", name);
			return ShadowMaterial();
		}
		return ShadowMaterial(materialId);
	}
	ShadowMaterial MaterialManager::GetDefaultShadowMaterial()
	{
		return GetShadowMaterial(s_defaultShadowMaterialId);
	}
	DeferredMaterial MaterialManager::TryGetDeferredMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
		{
			LOG_WARN("MaterialManager::TryGetDeferredMaterial(...) failed. Material '{}' not found, inaccessible, or not a deferred material.", name);
			return DeferredMaterial();
		}
		return DeferredMaterial(materialId);
	}
	ForwardMaterial MaterialManager::TryGetForwardMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::forward)
		{
			LOG_WARN("MaterialManager::TryGetForwardMaterial(...) failed. Material '{}' not found, inaccessible, or not a forward material.", name);
			return ForwardMaterial();
		}
		return ForwardMaterial(materialId);
	}



	// Deleter:
	void MaterialManager::DeleteMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		if (materialId == emberCommon::invalidMaterialId)
		{
			LOG_WARN("MaterialManager::DeleteMaterial(...) failed. Material '{}' not found or inaccessible.", name);
			return;
		}
		DeleteMaterial(materialId);
	}



	// Debugging:
	void MaterialManager::Print()
	{
		if (!s_isInitialized)
		{
			LOG_WARN("MaterialManager::Print() failed. Material manager is not initialized.");
			return;
		}

		LOG_TRACE("MaterialManager contents:");
		for (const std::pair<const std::string, uint32_t>& materialIdPair : s_materialIdsMap)
		{
			const std::string& name = materialIdPair.first;
			const uint32_t index = materialIdPair.second;
			const MaterialSlot& slot = s_materialSlots[index];
			const emberCommon::ResourceAccessRights& accessRights = slot.managedMaterial.accessRights;
			LOG_TRACE("  {}: index {}, generation {}, accessible {}, deletable {}, mutable {}", name, index, slot.generation, accessRights.isAccessible, accessRights.isDeletable, accessRights.isMutable);
		}
	}



	// Private methods:
	// Initialization/Cleanup:
	void MaterialManager::Init()
	{
		if (s_isInitialized)
			return;
		if (!GpuResourceFactory::s_isInitialized)
			throw std::runtime_error("MaterialManager::Init() failed. Gpu resource factory is not initialized.");

		LoadMaterialAssets(std::filesystem::path(ENGINE_SHADERS_DIR) / "materialAssets");
		DefaultGpuResources::InitializeBuiltInMaterials(
			TakeMaterialOwnership("outlineMaterial"),
			TakeMaterialOwnership("defaultShadowMaterial"),
			TakeMaterialOwnership("pbrDeferredLightingMaterial"),
			TakeMaterialOwnership("presentMaterial"));

		s_pIErrorMaterial = TryGetMaterialInterface(FindMaterialId("errorMaterial"));
		if (s_pIErrorMaterial == nullptr)
			throw std::runtime_error("MaterialManager::Init() failed. errorMaterial is missing.");
		s_pIErrorGizmoMaterial = TryGetMaterialInterface(FindMaterialId("errorGizmoMaterial"));
		if (s_pIErrorGizmoMaterial == nullptr || s_pIErrorGizmoMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("MaterialManager::Init() failed. errorGizmoMaterial is missing or is not a gizmo material.");

		s_isInitialized = true;
	}
	void MaterialManager::Clear()
	{
		if (!s_isInitialized)
			return;

		s_pIErrorMaterial = nullptr;
		s_pIErrorGizmoMaterial = nullptr;

		for (uint32_t index = 0; index < s_materialSlots.size(); index++)
		{
			MaterialSlot& slot = s_materialSlots[index];
			if (slot.managedMaterial.pMaterial == nullptr)
				continue;

			RetireMaterial(std::move(slot.managedMaterial.pOwnedMaterial));
			InvalidateMaterialSlot(index);
		}
		s_materialIdsMap.clear();
		s_defaultShadowMaterialId = emberCommon::invalidMaterialId;
		s_isInitialized = false;
	}



	// Creation/Cloning:
	emberCommon::MaterialId MaterialManager::CreateMaterial(const emberAssetLoader::MaterialAsset& materialAsset)
	{
		if (TryGetMaterialInterface(FindMaterialId(materialAsset.materialName)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", materialAsset.materialName);
			return emberCommon::invalidMaterialId;
		}

		MaterialShader materialShader = MaterialShaderManager::CreateMaterialShader(materialAsset);
		emberCommon::MaterialCreateInfo createInfo
		{
			materialAsset.GetMaterialPass(),
			std::monostate{},
			materialAsset.materialName
		};
		if (createInfo.materialPass == emberCommon::MaterialPass::gizmo)
			createInfo.renderMode = std::get<emberAssetLoader::MaterialAsset::GizmoSettings>(materialAsset.renderModeSettings).renderMode;
		else if (createInfo.materialPass == emberCommon::MaterialPass::forward)
			createInfo.renderMode = std::get<emberAssetLoader::MaterialAsset::ForwardSettings>(materialAsset.renderModeSettings).renderMode;

		std::unique_ptr<emberBackendInterface::IMaterial> pMaterial(GpuResourceFactory::CreateMaterial(materialShader.TryGetInterfaceHandle(), createInfo));
		if (pMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CreateMaterial(...) failed. Gpu resource factory returned nullptr for: " + materialAsset.materialName);
		if (pMaterial->GetMaterialPass() != createInfo.materialPass)
			throw std::runtime_error("MaterialManager::CreateMaterial(...) failed. Gpu resource factory returned a material with the wrong material pass: " + materialAsset.materialName);

		return AddMaterial(materialAsset.materialName, materialAsset.accessRights, materialShader.m_materialShaderId, std::move(pMaterial));
	}
	emberCommon::MaterialId MaterialManager::CloneMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::MaterialPass expectedMaterialPass, emberCommon::MaterialBindingCloneMode bindingCloneMode, const std::string& name)
	{
		emberBackendInterface::IMaterial* pSourceMaterial = TryGetMaterialInterface(sourceMaterialId);
		if (pSourceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CloneMaterial(...) failed. Source material is invalid or expired.");
		if (pSourceMaterial->GetMaterialPass() != expectedMaterialPass)
			throw std::runtime_error("MaterialManager::CloneMaterial(...) failed. Source material has the wrong material pass.");
		if (TryGetMaterialInterface(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}

		const emberCommon::MaterialShaderId* pMaterialShaderId = TryGetMaterialShaderId(sourceMaterialId);
		if (pMaterialShaderId == nullptr || MaterialShaderManager::TryGetMaterialShaderInterface(*pMaterialShaderId) == nullptr)
			throw std::runtime_error("MaterialManager::CloneMaterial(...) failed. Source material shader is invalid or expired.");

		emberCommon::MaterialCloneInfo cloneInfo{ bindingCloneMode, name };
		std::unique_ptr<emberBackendInterface::IMaterial> pMaterial(GpuResourceFactory::CloneMaterial(pSourceMaterial, cloneInfo));
		if (pMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CloneMaterial(...) failed. Gpu resource factory returned nullptr for: " + name);
		if (pMaterial->GetMaterialPass() != expectedMaterialPass)
			throw std::runtime_error("MaterialManager::CloneMaterial(...) failed. Gpu resource factory returned a material with the wrong material pass: " + name);

		emberCommon::ResourceAccessRights accessRights{ true, true, true };
		return AddMaterial(name, accessRights, *pMaterialShaderId, std::move(pMaterial));
	}



	// Getters:
	Material MaterialManager::GetMaterial(emberCommon::MaterialId materialId)
	{
		if (TryGetMaterialInterface(materialId) == nullptr)
			throw std::runtime_error("MaterialManager::GetMaterial(...) failed. Material is invalid or expired.");
		return Material(materialId);
	}
	GizmoMaterial MaterialManager::GetGizmoMaterial(emberCommon::MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("MaterialManager::GetGizmoMaterial(...) failed. Material is invalid, expired, or not a gizmo material.");
		return GizmoMaterial(materialId);
	}
	ShadowMaterial MaterialManager::GetShadowMaterial(emberCommon::MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::GetShadowMaterial(...) failed. Material is invalid, expired, or not a shadow material.");
		return ShadowMaterial(materialId);
	}
	ShadowMaterial MaterialManager::GetShadowMaterialForSurfaceMaterial(emberCommon::MaterialId surfaceMaterialId)
	{
		return GetShadowMaterial(TryGetShadowMaterialIdOfSurfaceMaterial(surfaceMaterialId));
	}
	DeferredMaterial MaterialManager::GetDeferredMaterial(emberCommon::MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
			throw std::runtime_error("MaterialManager::GetDeferredMaterial(...) failed. Material is invalid, expired, or not a deferred material.");
		return DeferredMaterial(materialId);
	}
	ForwardMaterial MaterialManager::GetForwardMaterial(emberCommon::MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("MaterialManager::GetForwardMaterial(...) failed. Material is invalid, expired, or not a forward material.");
		return ForwardMaterial(materialId);
	}
	emberCommon::MaterialId MaterialManager::TryGetMaterialId(const std::string& name)
	{
		emberCommon::MaterialId materialId = FindMaterialId(name);
		if (TryGetMaterialInterface(materialId) == nullptr || !s_materialSlots[materialId.index].managedMaterial.accessRights.isAccessible)
			return emberCommon::invalidMaterialId;
		return materialId;
	}
	emberBackendInterface::IMaterial* MaterialManager::TryGetMaterialInterface(emberCommon::MaterialId materialId)
	{
		if (materialId.index == emberCommon::invalidMaterialId.index || materialId.index >= s_materialSlots.size())
			return nullptr;

		const MaterialSlot& slot = s_materialSlots[materialId.index];
		if (slot.generation != materialId.generation)
			return nullptr;
		return slot.managedMaterial.pMaterial;
	}
	const std::string* MaterialManager::TryGetMaterialName(emberCommon::MaterialId materialId)
	{
		if (TryGetMaterialInterface(materialId) == nullptr)
			return nullptr;
		return &s_materialSlots[materialId.index].managedMaterial.name;
	}
	emberCommon::MaterialId MaterialManager::TryGetShadowMaterialIdOfSurfaceMaterial(emberCommon::MaterialId surfaceMaterialId)
	{
		emberBackendInterface::IMaterial* pSurfaceMaterial = TryGetMaterialInterface(surfaceMaterialId);
		if (pSurfaceMaterial == nullptr || !emberCommon::IsSurfaceMaterialPass(pSurfaceMaterial->GetMaterialPass()))
			return emberCommon::invalidMaterialId;

		emberCommon::MaterialId& shadowMaterialId = s_materialSlots[surfaceMaterialId.index].managedMaterial.shadowMaterialId;
		emberBackendInterface::IMaterial* pShadowMaterial = TryGetMaterialInterface(shadowMaterialId);
		if (pShadowMaterial == nullptr || pShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			shadowMaterialId = s_defaultShadowMaterialId;

		pShadowMaterial = TryGetMaterialInterface(shadowMaterialId);
		if (pShadowMaterial == nullptr || pShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			return emberCommon::invalidMaterialId;
		return shadowMaterialId;
	}
	const emberCommon::MaterialShaderId* MaterialManager::TryGetMaterialShaderId(emberCommon::MaterialId materialId)
	{
		if (TryGetMaterialInterface(materialId) == nullptr)
			return nullptr;
		return &s_materialSlots[materialId.index].managedMaterial.materialShaderId;
	}
	bool MaterialManager::IsMaterialMutable(emberCommon::MaterialId materialId)
	{
		return TryGetMaterialInterface(materialId) != nullptr && s_materialSlots[materialId.index].managedMaterial.accessRights.isMutable;
	}



	// Setters:
	void MaterialManager::SetShadowMaterial(emberCommon::MaterialId surfaceMaterialId, emberCommon::MaterialId shadowMaterialId)
	{
		emberBackendInterface::IMaterial* pSurfaceMaterial = TryGetMaterialInterface(surfaceMaterialId);
		if (pSurfaceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Surface material is invalid or expired.");
		if (!emberCommon::IsSurfaceMaterialPass(pSurfaceMaterial->GetMaterialPass()))
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Material is not a deferred or forward material.");

		emberBackendInterface::IMaterial* pShadowMaterial = TryGetMaterialInterface(shadowMaterialId);
		if (pShadowMaterial == nullptr)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Shadow material is invalid or expired.");
		if (pShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Material is not a shadow material.");

		s_materialSlots[surfaceMaterialId.index].managedMaterial.shadowMaterialId = shadowMaterialId;
	}
	void MaterialManager::ResetShadowMaterial(emberCommon::MaterialId surfaceMaterialId)
	{
		emberBackendInterface::IMaterial* pSurfaceMaterial = TryGetMaterialInterface(surfaceMaterialId);
		if (pSurfaceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::ResetShadowMaterial(...) failed. Surface material is invalid or expired.");
		if (!emberCommon::IsSurfaceMaterialPass(pSurfaceMaterial->GetMaterialPass()))
			throw std::runtime_error("MaterialManager::ResetShadowMaterial(...) failed. Material is not a deferred or forward material.");

		s_materialSlots[surfaceMaterialId.index].managedMaterial.shadowMaterialId = s_defaultShadowMaterialId;
	}



	// Deleter:
	void MaterialManager::DeleteMaterial(emberCommon::MaterialId materialId)
	{
		if (TryGetMaterialInterface(materialId) == nullptr)
			return;

		MaterialSlot& slot = s_materialSlots[materialId.index];
		if (!slot.managedMaterial.accessRights.isDeletable)
		{
			LOG_WARN("MaterialManager::DeleteMaterial(...) failed. Material '{}' is pinned until shutdown.", slot.managedMaterial.name);
			return;
		}

		s_materialIdsMap.erase(slot.managedMaterial.name);
		RetireMaterial(std::move(slot.managedMaterial.pOwnedMaterial));
		InvalidateMaterialSlot(materialId.index);
	}



	// Management:
	emberCommon::MaterialId MaterialManager::AddMaterial(const std::string& name, const emberCommon::ResourceAccessRights& accessRights, emberCommon::MaterialShaderId materialShaderId, std::unique_ptr<emberBackendInterface::IMaterial> pMaterial)
	{
		if (pMaterial == nullptr)
			throw std::runtime_error("MaterialManager::AddMaterial(...) failed. pMaterial is nullptr.");
		if (MaterialShaderManager::TryGetMaterialShaderInterface(materialShaderId) == nullptr)
			throw std::runtime_error("MaterialManager::AddMaterial(...) failed. MaterialShader is invalid or expired.");
		if (TryGetMaterialInterface(FindMaterialId(name)) != nullptr)
			throw std::runtime_error("MaterialManager::AddMaterial(...) failed. Material already exists: " + name);

		emberCommon::MaterialId shadowMaterialId = emberCommon::IsSurfaceMaterialPass(pMaterial->GetMaterialPass()) ? s_defaultShadowMaterialId : emberCommon::invalidMaterialId;
		emberCommon::MaterialId materialId;
		if (s_freeMaterialIds.empty())
		{
			if (s_materialSlots.size() >= emberCommon::invalidMaterialId.index)
				throw std::runtime_error("MaterialManager::AddMaterial(...) failed. Material id limit reached.");
			materialId.index = static_cast<uint32_t>(s_materialSlots.size());
			s_materialSlots.emplace_back(1, ManagedMaterial(name, accessRights, materialShaderId, shadowMaterialId, std::move(pMaterial)));
		}
		else
		{
			materialId.index = s_freeMaterialIds.back();
			s_freeMaterialIds.pop_back();

			MaterialSlot& slot = s_materialSlots[materialId.index];
			slot.managedMaterial.name = name;
			slot.managedMaterial.accessRights = accessRights;
			slot.managedMaterial.materialShaderId = materialShaderId;
			slot.managedMaterial.shadowMaterialId = shadowMaterialId;
			slot.managedMaterial.pOwnedMaterial = std::move(pMaterial);
			slot.managedMaterial.pMaterial = slot.managedMaterial.pOwnedMaterial.get();
		}

		materialId.generation = s_materialSlots[materialId.index].generation;
		s_materialIdsMap[name] = materialId.index;
		return materialId;
	}
	std::unique_ptr<emberBackendInterface::IMaterial> MaterialManager::TakeMaterialOwnership(const std::string& name)
	{
		emberCommon::MaterialId materialId = FindMaterialId(name);
		if (TryGetMaterialInterface(materialId) == nullptr)
			throw std::runtime_error("MaterialManager::TakeMaterialOwnership(...) failed. Material not found: " + name);

		MaterialSlot& slot = s_materialSlots[materialId.index];
		if (slot.managedMaterial.pOwnedMaterial == nullptr)
			throw std::runtime_error("MaterialManager::TakeMaterialOwnership(...) failed. Material ownership was already transferred: " + name);

		// Keep the slot and its non-owning pMaterial pointer valid after transferring ownership.
		// Core handles, especially the default shadow material ids stored by surface materials, continue to reference this slot.
		return std::move(slot.managedMaterial.pOwnedMaterial);
	}
	void MaterialManager::RetireMaterial(std::unique_ptr<emberBackendInterface::IMaterial> pMaterial)
	{
		if (pMaterial != nullptr)
			GpuResourceFactory::RetireMaterial(pMaterial.release());
	}
	emberCommon::MaterialId MaterialManager::FindMaterialId(const std::string& name)
	{
		std::unordered_map<std::string, uint32_t>::const_iterator iterator = s_materialIdsMap.find(name);
		if (iterator == s_materialIdsMap.end())
			return emberCommon::invalidMaterialId;

		const uint32_t index = iterator->second;
		return emberCommon::MaterialId{ index, s_materialSlots[index].generation };
	}
	void MaterialManager::InvalidateMaterialSlot(uint32_t index)
	{
		MaterialSlot& slot = s_materialSlots[index];
		slot.managedMaterial.name.clear();
		slot.managedMaterial.accessRights = { false, false, false };
		slot.managedMaterial.materialShaderId = emberCommon::invalidMaterialShaderId;
		slot.managedMaterial.shadowMaterialId = emberCommon::invalidMaterialId;
		slot.managedMaterial.pOwnedMaterial.reset();
		slot.managedMaterial.pMaterial = nullptr;
		slot.generation++;
		if (slot.generation != emberCommon::invalidMaterialId.generation)
			s_freeMaterialIds.push_back(index);
	}
}
