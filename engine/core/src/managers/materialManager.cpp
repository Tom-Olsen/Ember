#include "materialManager.h"
#include "iGpuResourceFactory.h"
#include "iMaterial.h"
#include "iMaterialManager.h"
#include "logger.h"
#include "materialAsset.h"
#include "materialAssetLoader.h"
#include "renderer.h"
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <vector>



namespace emberCore
{
	// Static members:
	emberBackendInterface::IMaterialManager* MaterialManager::s_pIMaterialManager = nullptr;
	emberBackendInterface::IMaterial* MaterialManager::s_pIErrorMaterial = nullptr;
	emberBackendInterface::IMaterial* MaterialManager::s_pIErrorGizmoMaterial = nullptr;



	// Public methods:
	// Asset loading:
	void MaterialManager::LoadMaterialAssets(const std::filesystem::path& directoryPath)
	{
		// Error handling:
		if (s_pIMaterialManager == nullptr)
			throw std::runtime_error("MaterialManager::LoadMaterialAssets(...) failed. Material manager is not initialized.");
		if (!std::filesystem::is_directory(directoryPath))
			throw std::runtime_error("MaterialManager::LoadMaterialAssets(...) failed. Directory does not exist: " + directoryPath.string());

		// Collect all asset paths:
		std::vector<std::filesystem::path> assetPaths;
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_regular_file() && entry.path().filename().string().ends_with(".materialAsset.json"))
				assetPaths.push_back(entry.path());
		}
		std::sort(assetPaths.begin(), assetPaths.end());

		// Create all material assets:
		std::vector<emberAssetLoader::MaterialAsset> materialAssets;
		materialAssets.reserve(assetPaths.size());
		for (const std::filesystem::path& assetPath : assetPaths)
			materialAssets.push_back(emberAssetLoader::MaterialAssetLoader::Load(assetPath));

		// Create all materials:
		for (const emberAssetLoader::MaterialAsset& materialAsset : materialAssets)
			s_pIMaterialManager->CreateMaterial(materialAsset);
	}



	// Cloners:
	GizmoMaterial MaterialManager::CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneGizmoMaterial(sourceMaterial.m_materialId, sourceMaterial.GetRenderMode(), name);
		return materialId.index == emberCommon::invalidMaterialId.index ? GizmoMaterial() : GizmoMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneGizmoMaterial(sourceMaterial.m_materialId, renderMode, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? GizmoMaterial() : GizmoMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterialWithDefaultBindings(const GizmoMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneGizmoMaterialWithDefaultBindings(sourceMaterial.m_materialId, sourceMaterial.GetRenderMode(), name);
		return materialId.index == emberCommon::invalidMaterialId.index ? GizmoMaterial() : GizmoMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterialWithDefaultBindings(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneGizmoMaterialWithDefaultBindings(sourceMaterial.m_materialId, renderMode, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? GizmoMaterial() : GizmoMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::CloneShadowMaterial(const ShadowMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneShadowMaterial(sourceMaterial.m_materialId, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? ShadowMaterial() : ShadowMaterial{ materialId };
	}
	DeferredMaterial MaterialManager::CloneDeferredGeometryMaterial(const DeferredMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneDeferredGeometryMaterial(sourceMaterial.m_materialId, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? DeferredMaterial() : DeferredMaterial{ materialId };
	}
	DeferredMaterial MaterialManager::CloneDeferredGeometryMaterialWithDefaultBindings(const DeferredMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneDeferredGeometryMaterialWithDefaultBindings(sourceMaterial.m_materialId, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? DeferredMaterial() : DeferredMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CloneForwardMaterial(const ForwardMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneForwardMaterial(sourceMaterial.m_materialId, sourceMaterial.GetRenderMode(), name);
		return materialId.index == emberCommon::invalidMaterialId.index ? ForwardMaterial() : ForwardMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CloneForwardMaterial(const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneForwardMaterial(sourceMaterial.m_materialId, renderMode, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? ForwardMaterial() : ForwardMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CloneForwardMaterialWithDefaultBindings(const ForwardMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneForwardMaterialWithDefaultBindings(sourceMaterial.m_materialId, sourceMaterial.GetRenderMode(), name);
		return materialId.index == emberCommon::invalidMaterialId.index ? ForwardMaterial() : ForwardMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CloneForwardMaterialWithDefaultBindings(const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneForwardMaterialWithDefaultBindings(sourceMaterial.m_materialId, renderMode, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? ForwardMaterial() : ForwardMaterial{ materialId };
	}



	// Getters:
	Material MaterialManager::TryGetMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		if (TryGetMaterialInterface(materialId) == nullptr)
		{
			LOG_WARN("MaterialManager::TryGetMaterial(...) failed. Material '{}' not found or expired.", name);
			return GetMaterial(TryGetMaterialId("errorMaterial"));
		}
		return Material{ materialId };
	}
	GizmoMaterial MaterialManager::TryGetGizmoMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr)
		{
			LOG_WARN("MaterialManager::TryGetGizmoMaterial(...) failed. Material '{}' not found or expired.", name);
			return GizmoMaterial();
		}
		if (pMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
		{
			LOG_WARN("MaterialManager::TryGetGizmoMaterial(...) failed. Material '{}' is not a gizmo material.", name);
			return GizmoMaterial();
		}
		return GizmoMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::TryGetShadowMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr)
		{
			LOG_WARN("MaterialManager::TryGetShadowMaterial(...) failed. Material '{}' not found or expired.", name);
			return ShadowMaterial();
		}
		if (pMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
		{
			LOG_WARN("MaterialManager::TryGetShadowMaterial(...) failed. Material '{}' is not a shadow material.", name);
			return ShadowMaterial();
		}
		return ShadowMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::GetDefaultShadowMaterial()
	{
		return ShadowMaterial{ s_pIMaterialManager->GetDefaultShadowMaterialId() };
	}
	DeferredMaterial MaterialManager::TryGetDeferredMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr)
		{
			LOG_WARN("MaterialManager::TryGetDeferredMaterial(...) failed. Material '{}' not found or expired.", name);
			return DeferredMaterial();
		}
		if (pMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
		{
			LOG_WARN("MaterialManager::TryGetDeferredMaterial(...) failed. Material '{}' is not a deferred material.", name);
			return DeferredMaterial();
		}
		return DeferredMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::TryGetForwardMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr)
		{
			LOG_WARN("MaterialManager::TryGetForwardMaterial(...) failed. Material '{}' not found or expired.", name);
			return ForwardMaterial();
		}
		if (pMaterial->GetMaterialPass() != emberCommon::MaterialPass::forward)
		{
			LOG_WARN("MaterialManager::TryGetForwardMaterial(...) failed. Material '{}' is not a forward material.", name);
			return ForwardMaterial();
		}
		return ForwardMaterial{ materialId };
	}



	// Deleter:
	void MaterialManager::DeleteMaterial(const std::string& name)
	{
		emberCommon::MaterialId materialId = TryGetMaterialId(name);
		if (materialId.index == emberCommon::invalidMaterialId.index)
		{
			LOG_WARN("MaterialManager::DeleteMaterial(...) failed. Material '{}' not found or expired.", name);
			return;
		}
		DeleteMaterial(materialId);
	}



	// Debugging:
	void MaterialManager::Print()
	{
		s_pIMaterialManager->Print();
	}



	// Private methods:
	// Initialization/Cleanup:
	void MaterialManager::Init()
	{
		if (s_pIMaterialManager != nullptr)
			return;
		if (Renderer::s_pIGpuResourceFactory == nullptr)
			throw std::runtime_error("MaterialManager::Init() failed. Gpu resource factory is not initialized.");

		s_pIMaterialManager = Renderer::s_pIGpuResourceFactory->GetMaterialManager();
		if (s_pIMaterialManager == nullptr)
			throw std::runtime_error("MaterialManager::Init() failed. Gpu resource factory returned a nullptr material manager.");

		// Load engines default materials:
		LoadMaterialAssets(std::filesystem::path(ENGINE_SHADERS_DIR) / "materialAssets");
		s_pIMaterialManager->InitializeDefaultMaterials();
		s_pIErrorMaterial = TryGetMaterialInterface(TryGetMaterialId("errorMaterial"));
		if (s_pIErrorMaterial == nullptr)
			throw std::runtime_error("MaterialManager::Init() failed. errorMaterial is missing.");
		s_pIErrorGizmoMaterial = TryGetMaterialInterface(TryGetMaterialId("errorGizmoMaterial"));
		if (s_pIErrorGizmoMaterial == nullptr || s_pIErrorGizmoMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("MaterialManager::Init() failed. errorGizmoMaterial is missing or is not a gizmo material.");
	}
	void MaterialManager::Clear()
	{
		s_pIErrorMaterial = nullptr;
		s_pIErrorGizmoMaterial = nullptr;
		s_pIMaterialManager = nullptr;
	}



	// Getters:
	Material MaterialManager::GetMaterial(emberCommon::MaterialId materialId)
	{
		if (TryGetMaterialInterface(materialId) == nullptr)
			throw std::runtime_error("MaterialManager::GetMaterial(...) failed. Material is invalid or expired.");
		return Material{ materialId };
	}
	GizmoMaterial MaterialManager::GetGizmoMaterial(emberCommon::MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("MaterialManager::GetGizmoMaterial(...) failed. Material is invalid, expired, or not a gizmo material.");
		return GizmoMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::GetShadowMaterial(emberCommon::MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::GetShadowMaterial(...) failed. Material is invalid, expired, or not a shadow material.");
		return ShadowMaterial{ materialId };
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
		return DeferredMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::GetForwardMaterial(emberCommon::MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pMaterial = TryGetMaterialInterface(materialId);
		if (pMaterial == nullptr || pMaterial->GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("MaterialManager::GetForwardMaterial(...) failed. Material is invalid, expired, or not a forward material.");
		return ForwardMaterial{ materialId };
	}
	emberCommon::MaterialId MaterialManager::TryGetMaterialId(const std::string& name)
	{
		return s_pIMaterialManager->TryGetMaterialId(name);
	}
	emberBackendInterface::IMaterial* MaterialManager::TryGetMaterialInterface(emberCommon::MaterialId materialId)
	{
		return s_pIMaterialManager->TryGetMaterial(materialId);
	}
	const std::string* MaterialManager::TryGetMaterialName(emberCommon::MaterialId materialId)
	{
		return s_pIMaterialManager->TryGetMaterialName(materialId);
	}
	emberCommon::MaterialId MaterialManager::TryGetShadowMaterialIdOfSurfaceMaterial(emberCommon::MaterialId surfaceMaterialId)
	{
		return s_pIMaterialManager->TryGetShadowMaterialId(surfaceMaterialId);
	}
	bool MaterialManager::IsMaterialMutable(emberCommon::MaterialId materialId)
	{
		return s_pIMaterialManager->IsMaterialMutable(materialId);
	}



	// Setters:
	void MaterialManager::ResetShadowMaterial(emberCommon::MaterialId surfaceMaterialId)
	{
		s_pIMaterialManager->ResetShadowMaterial(surfaceMaterialId);
	}
	void MaterialManager::SetShadowMaterial(emberCommon::MaterialId surfaceMaterialId, emberCommon::MaterialId shadowMaterialId)
	{
		s_pIMaterialManager->SetShadowMaterial(surfaceMaterialId, shadowMaterialId);
	}



	// Deleter:
	void MaterialManager::DeleteMaterial(emberCommon::MaterialId materialId)
	{
		s_pIMaterialManager->DeleteMaterial(materialId);
	}
}