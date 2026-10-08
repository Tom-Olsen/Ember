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
#include <optional>
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
	emberDataStructures::NamedSlotMap<emberCommon::MaterialId, MaterialManager::ManagedMaterial> MaterialManager::s_materialSlotMap;



	// Managed material methods:
	MaterialManager::ManagedMaterial::ManagedMaterial()
		: accessRights{ false, false, false }
		, materialShaderId(emberCommon::invalidMaterialShaderId)
		, shadowMaterialId(emberCommon::invalidMaterialId)
		, pIMaterial(nullptr)
	{

	}
	MaterialManager::ManagedMaterial::ManagedMaterial(const emberCommon::ResourceAccessRights& accessRights, emberCommon::MaterialShaderId materialShaderId, emberCommon::MaterialId shadowMaterialId, std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial)
		: accessRights(accessRights)
		, materialShaderId(materialShaderId)
		, shadowMaterialId(shadowMaterialId)
		, pOwnedIMaterial(std::move(pIMaterial))
		, pIMaterial(pOwnedIMaterial.get())
	{

	}
	MaterialManager::ManagedMaterial::~ManagedMaterial() = default;
	MaterialManager::ManagedMaterial::ManagedMaterial(ManagedMaterial&&) noexcept = default;
	MaterialManager::ManagedMaterial& MaterialManager::ManagedMaterial::operator=(ManagedMaterial&&) noexcept = default;



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
		for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(directoryPath))
		{
			if (directoryEntry.is_regular_file() && directoryEntry.path().filename().string().ends_with(".materialAsset.json"))
				assetPaths.push_back(directoryEntry.path());
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
			for (emberCommon::MaterialId materialId : s_materialSlotMap.GetActiveIds())
			{
				ManagedMaterial* pManagedMaterial = s_materialSlotMap.TryGetValue(materialId);
				if (pManagedMaterial->pIMaterial != nullptr && emberCommon::IsSurfaceMaterialPass(pManagedMaterial->pIMaterial->GetMaterialPass()) && pManagedMaterial->shadowMaterialId == emberCommon::invalidMaterialId)
					pManagedMaterial->shadowMaterialId = s_defaultShadowMaterialId;
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
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr)
			return GizmoMaterial();
		pIMaterial->SetGizmoRenderMode(renderMode);
		return GizmoMaterial(materialId);
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterialWithDefaultBindings(const GizmoMaterial& sourceMaterial, const std::string& name)
	{
		return CloneGizmoMaterialWithDefaultBindings(sourceMaterial, sourceMaterial.GetRenderMode(), name);
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterialWithDefaultBindings(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = CloneMaterial(sourceMaterial.m_materialId, emberCommon::MaterialPass::gizmo, emberCommon::MaterialBindingCloneMode::defaultBindings, name);
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr)
			return GizmoMaterial();
		pIMaterial->SetGizmoRenderMode(renderMode);
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
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr)
			return ForwardMaterial();
		pIMaterial->SetForwardRenderMode(renderMode);
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
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr)
			return ForwardMaterial();
		pIMaterial->SetForwardRenderMode(renderMode);
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
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr || pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
		{
			LOG_WARN("MaterialManager::TryGetGizmoMaterial(...) failed. Material '{}' not found, inaccessible, or not a gizmo material.", name);
			return GizmoMaterial();
		}
		return GizmoMaterial(materialId);
	}
	ShadowMaterial MaterialManager::TryGetShadowMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr || pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
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
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr || pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
		{
			LOG_WARN("MaterialManager::TryGetDeferredMaterial(...) failed. Material '{}' not found, inaccessible, or not a deferred material.", name);
			return DeferredMaterial();
		}
		return DeferredMaterial(materialId);
	}
	ForwardMaterial MaterialManager::TryGetForwardMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr || pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::forward)
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
		for (emberCommon::MaterialId materialId : s_materialSlotMap.GetActiveIds())
		{
			const ManagedMaterial* pManagedMaterial = s_materialSlotMap.TryGetValue(materialId);
			const emberCommon::ResourceAccessRights& accessRights = pManagedMaterial->accessRights;
			std::string materialName = *s_materialSlotMap.TryGetName(materialId);
			LOG_TRACE("  {}: index {}, generation {}, accessible {}, deletable {}, mutable {}", materialName, materialId.index, materialId.generation, accessRights.isAccessible, accessRights.isDeletable, accessRights.isMutable);
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

		for (emberCommon::MaterialId materialId : s_materialSlotMap.GetActiveIds())
		{
			std::optional<ManagedMaterial> managedMaterial = s_materialSlotMap.Remove(materialId);
			RetireMaterial(std::move(managedMaterial->pOwnedIMaterial));
		}
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
		emberCommon::MaterialCreateInfo materialCreateInfo
		{
			materialAsset.GetMaterialPass(),
			std::monostate{},
			materialAsset.materialName
		};
		if (materialCreateInfo.materialPass == emberCommon::MaterialPass::gizmo)
			materialCreateInfo.renderMode = std::get<emberAssetLoader::MaterialAsset::GizmoSettings>(materialAsset.renderModeSettings).renderMode;
		else if (materialCreateInfo.materialPass == emberCommon::MaterialPass::forward)
			materialCreateInfo.renderMode = std::get<emberAssetLoader::MaterialAsset::ForwardSettings>(materialAsset.renderModeSettings).renderMode;

		std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial(GpuResourceFactory::CreateMaterial(materialShader.TryGetInterfaceHandle(), materialCreateInfo));
		if (pIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CreateMaterial(...) failed. Gpu resource factory returned nullptr for: " + materialAsset.materialName);
		if (pIMaterial->GetMaterialPass() != materialCreateInfo.materialPass)
			throw std::runtime_error("MaterialManager::CreateMaterial(...) failed. Gpu resource factory returned a material with the wrong material pass: " + materialAsset.materialName);

		return AddMaterial(materialAsset.materialName, materialAsset.accessRights, materialShader.m_materialShaderId, std::move(pIMaterial));
	}
	emberCommon::MaterialId MaterialManager::CloneMaterial(emberCommon::MaterialId sourceMaterialId, emberCommon::MaterialPass expectedMaterialPass, emberCommon::MaterialBindingCloneMode bindingCloneMode, const std::string& name)
	{
		emberBackendInterface::IMaterial* pSourceIMaterial = TryGetMaterialInterface(sourceMaterialId);
		if (pSourceIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CloneMaterial(...) failed. Source material is invalid or expired.");
		if (pSourceIMaterial->GetMaterialPass() != expectedMaterialPass)
			throw std::runtime_error("MaterialManager::CloneMaterial(...) failed. Source material has the wrong material pass.");
		if (TryGetMaterialInterface(FindMaterialId(name)) != nullptr)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return emberCommon::invalidMaterialId;
		}

		emberCommon::MaterialShaderId materialShaderId = TryGetMaterialShaderId(sourceMaterialId);
		if (materialShaderId == emberCommon::invalidMaterialShaderId || MaterialShaderManager::TryGetMaterialShaderInterface(materialShaderId) == nullptr)
			throw std::runtime_error("MaterialManager::CloneMaterial(...) failed. Source material shader is invalid or expired.");

		emberCommon::MaterialCloneInfo materialCloneInfo{ bindingCloneMode, name };
		std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial(GpuResourceFactory::CloneMaterial(pSourceIMaterial, materialCloneInfo));
		if (pIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::CloneMaterial(...) failed. Gpu resource factory returned nullptr for: " + name);
		if (pIMaterial->GetMaterialPass() != expectedMaterialPass)
			throw std::runtime_error("MaterialManager::CloneMaterial(...) failed. Gpu resource factory returned a material with the wrong material pass: " + name);

		emberCommon::ResourceAccessRights accessRights{ true, true, true };
		return AddMaterial(name, accessRights, materialShaderId, std::move(pIMaterial));
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
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr || pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("MaterialManager::GetGizmoMaterial(...) failed. Material is invalid, expired, or not a gizmo material.");
		return GizmoMaterial(materialId);
	}
	ShadowMaterial MaterialManager::GetShadowMaterial(emberCommon::MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr || pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::GetShadowMaterial(...) failed. Material is invalid, expired, or not a shadow material.");
		return ShadowMaterial(materialId);
	}
	ShadowMaterial MaterialManager::GetShadowMaterialForSurfaceMaterial(emberCommon::MaterialId surfaceMaterialId)
	{
		return GetShadowMaterial(TryGetShadowMaterialIdOfSurfaceMaterial(surfaceMaterialId));
	}
	DeferredMaterial MaterialManager::GetDeferredMaterial(emberCommon::MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr || pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
			throw std::runtime_error("MaterialManager::GetDeferredMaterial(...) failed. Material is invalid, expired, or not a deferred material.");
		return DeferredMaterial(materialId);
	}
	ForwardMaterial MaterialManager::GetForwardMaterial(emberCommon::MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = TryGetMaterialInterface(materialId);
		if (pIMaterial == nullptr || pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("MaterialManager::GetForwardMaterial(...) failed. Material is invalid, expired, or not a forward material.");
		return ForwardMaterial(materialId);
	}
	emberCommon::MaterialId MaterialManager::TryGetMaterialId(const std::string& name)
	{
		emberCommon::MaterialId materialId = FindMaterialId(name);
		const ManagedMaterial* pManagedMaterial = s_materialSlotMap.TryGetValue(materialId);
		if (pManagedMaterial == nullptr || pManagedMaterial->pIMaterial == nullptr || !pManagedMaterial->accessRights.isAccessible)
			return emberCommon::invalidMaterialId;
		return materialId;
	}
	emberBackendInterface::IMaterial* MaterialManager::TryGetMaterialInterface(emberCommon::MaterialId materialId)
	{
		ManagedMaterial* pManagedMaterial = s_materialSlotMap.TryGetValue(materialId);
		return pManagedMaterial != nullptr ? pManagedMaterial->pIMaterial : nullptr;
	}
	std::optional<std::string> MaterialManager::TryGetMaterialName(emberCommon::MaterialId materialId)
	{
		if (TryGetMaterialInterface(materialId) == nullptr)
			return std::nullopt;
		return s_materialSlotMap.TryGetName(materialId);
	}
	emberCommon::MaterialId MaterialManager::TryGetShadowMaterialIdOfSurfaceMaterial(emberCommon::MaterialId surfaceMaterialId)
	{
		emberBackendInterface::IMaterial* pSurfaceIMaterial = TryGetMaterialInterface(surfaceMaterialId);
		if (pSurfaceIMaterial == nullptr || !emberCommon::IsSurfaceMaterialPass(pSurfaceIMaterial->GetMaterialPass()))
			return emberCommon::invalidMaterialId;

		ManagedMaterial* pManagedMaterial = s_materialSlotMap.TryGetValue(surfaceMaterialId);
		emberCommon::MaterialId& shadowMaterialId = pManagedMaterial->shadowMaterialId;
		emberBackendInterface::IMaterial* pShadowIMaterial = TryGetMaterialInterface(shadowMaterialId);
		if (pShadowIMaterial == nullptr || pShadowIMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			shadowMaterialId = s_defaultShadowMaterialId;

		pShadowIMaterial = TryGetMaterialInterface(shadowMaterialId);
		if (pShadowIMaterial == nullptr || pShadowIMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			return emberCommon::invalidMaterialId;
		return shadowMaterialId;
	}
	emberCommon::MaterialShaderId MaterialManager::TryGetMaterialShaderId(emberCommon::MaterialId materialId)
	{
		const ManagedMaterial* pManagedMaterial = s_materialSlotMap.TryGetValue(materialId);
		return pManagedMaterial != nullptr && pManagedMaterial->pIMaterial != nullptr ? pManagedMaterial->materialShaderId : emberCommon::invalidMaterialShaderId;
	}
	bool MaterialManager::IsMaterialMutable(emberCommon::MaterialId materialId)
	{
		const ManagedMaterial* pManagedMaterial = s_materialSlotMap.TryGetValue(materialId);
		return pManagedMaterial != nullptr && pManagedMaterial->pIMaterial != nullptr && pManagedMaterial->accessRights.isMutable;
	}



	// Setters:
	void MaterialManager::SetShadowMaterial(emberCommon::MaterialId surfaceMaterialId, emberCommon::MaterialId shadowMaterialId)
	{
		emberBackendInterface::IMaterial* pSurfaceIMaterial = TryGetMaterialInterface(surfaceMaterialId);
		if (pSurfaceIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Surface material is invalid or expired.");
		if (!emberCommon::IsSurfaceMaterialPass(pSurfaceIMaterial->GetMaterialPass()))
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Material is not a deferred or forward material.");

		emberBackendInterface::IMaterial* pShadowIMaterial = TryGetMaterialInterface(shadowMaterialId);
		if (pShadowIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Shadow material is invalid or expired.");
		if (pShadowIMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Material is not a shadow material.");

		s_materialSlotMap.TryGetValue(surfaceMaterialId)->shadowMaterialId = shadowMaterialId;
	}
	void MaterialManager::ResetShadowMaterial(emberCommon::MaterialId surfaceMaterialId)
	{
		emberBackendInterface::IMaterial* pSurfaceIMaterial = TryGetMaterialInterface(surfaceMaterialId);
		if (pSurfaceIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::ResetShadowMaterial(...) failed. Surface material is invalid or expired.");
		if (!emberCommon::IsSurfaceMaterialPass(pSurfaceIMaterial->GetMaterialPass()))
			throw std::runtime_error("MaterialManager::ResetShadowMaterial(...) failed. Material is not a deferred or forward material.");

		s_materialSlotMap.TryGetValue(surfaceMaterialId)->shadowMaterialId = s_defaultShadowMaterialId;
	}



	// Deleter:
	void MaterialManager::DeleteMaterial(emberCommon::MaterialId materialId)
	{
		ManagedMaterial* pManagedMaterial = s_materialSlotMap.TryGetValue(materialId);
		if (pManagedMaterial == nullptr || pManagedMaterial->pIMaterial == nullptr)
			return;
		if (!pManagedMaterial->accessRights.isDeletable)
		{
			LOG_WARN("MaterialManager::DeleteMaterial(...) failed. Material '{}' is pinned until shutdown.", *s_materialSlotMap.TryGetName(materialId));
			return;
		}

		std::optional<ManagedMaterial> managedMaterial = s_materialSlotMap.Remove(materialId);
		RetireMaterial(std::move(managedMaterial->pOwnedIMaterial));
	}



	// Management:
	emberCommon::MaterialId MaterialManager::AddMaterial(const std::string& name, const emberCommon::ResourceAccessRights& accessRights, emberCommon::MaterialShaderId materialShaderId, std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial)
	{
		if (pIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::AddMaterial(...) failed. pIMaterial is nullptr.");
		if (MaterialShaderManager::TryGetMaterialShaderInterface(materialShaderId) == nullptr)
			throw std::runtime_error("MaterialManager::AddMaterial(...) failed. MaterialShader is invalid or expired.");
		if (TryGetMaterialInterface(FindMaterialId(name)) != nullptr)
			throw std::runtime_error("MaterialManager::AddMaterial(...) failed. Material already exists: " + name);

		emberCommon::MaterialId shadowMaterialId = emberCommon::IsSurfaceMaterialPass(pIMaterial->GetMaterialPass()) ? s_defaultShadowMaterialId : emberCommon::invalidMaterialId;
		return s_materialSlotMap.Add(name, ManagedMaterial(accessRights, materialShaderId, shadowMaterialId, std::move(pIMaterial)));
	}
	std::unique_ptr<emberBackendInterface::IMaterial> MaterialManager::TakeMaterialOwnership(const std::string& name)
	{
		emberCommon::MaterialId materialId = FindMaterialId(name);
		ManagedMaterial* pManagedMaterial = s_materialSlotMap.TryGetValue(materialId);
		if (pManagedMaterial == nullptr || pManagedMaterial->pIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::TakeMaterialOwnership(...) failed. Material not found: " + name);
		if (pManagedMaterial->pOwnedIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::TakeMaterialOwnership(...) failed. Material ownership was already transferred: " + name);

		// Keep the slot and its non-owning pIMaterial pointer valid after transferring ownership.
		// Core handles, especially the default shadow material ids stored by surface materials, continue to reference this slot.
		return std::move(pManagedMaterial->pOwnedIMaterial);
	}
	void MaterialManager::RetireMaterial(std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial)
	{
		if (pIMaterial != nullptr)
			GpuResourceFactory::RetireMaterial(pIMaterial.release());
	}
	emberCommon::MaterialId MaterialManager::FindMaterialId(const std::string& name)
	{
		return s_materialSlotMap.Find(name);
	}
}