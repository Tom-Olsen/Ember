#include "materialManager.h"
#include "iMaterial.h"
#include "iMaterialManager.h"
#include "iRenderer.h"
#include "logger.h"
#include "materialAssetLoader.h"
#include "materialShader.h"
#include "materialShaderManager.h"
#include "renderer.h"
#include <stdexcept>



namespace emberCore
{
	// Static members:
	std::unique_ptr<emberBackendInterface::IMaterialManager> MaterialManager::s_pIMaterialManager;



	// Public methods:
	// Creators:
	Material MaterialManager::CreateMaterial(const emberAssetLoader::MaterialAsset& materialAsset)
	{
		const std::filesystem::path& vertexSpv = materialAsset.shaderStagePaths[static_cast<size_t>(emberCommon::ShaderStage::vertex)];
		const std::filesystem::path& fragmentSpv = materialAsset.shaderStagePaths[static_cast<size_t>(emberCommon::ShaderStage::fragment)];

		switch (materialAsset.GetMaterialPass())
		{
			case emberCommon::MaterialPass::gizmo:
			{
				MaterialShader materialShader = MaterialShaderManager::CreateGizmoMaterialShader(vertexSpv, fragmentSpv, materialAsset.materialShaderName);
				const emberAssetLoader::MaterialAsset::GizmoSettings& settings = std::get<emberAssetLoader::MaterialAsset::GizmoSettings>(materialAsset.renderModeSettings);
				return CreateGizmoMaterial(settings.renderMode, materialShader, materialAsset.materialName);
			}
			case emberCommon::MaterialPass::shadow:
			{
				MaterialShader materialShader = MaterialShaderManager::CreateShadowMaterialShader(vertexSpv, materialAsset.materialShaderName);
				return CreateShadowMaterial(materialShader, materialAsset.materialName);
			}
			case emberCommon::MaterialPass::deferredGeometry:
			{
				MaterialShader materialShader = MaterialShaderManager::CreateDeferredGeometryMaterialShader(vertexSpv, fragmentSpv, materialAsset.materialShaderName);
				return CreateDeferredGeometryMaterial(materialShader, materialAsset.materialName);
			}
			case emberCommon::MaterialPass::forward:
			{
				MaterialShader materialShader = MaterialShaderManager::CreateForwardMaterialShader(vertexSpv, fragmentSpv, materialAsset.materialShaderName);
				const emberAssetLoader::MaterialAsset::ForwardSettings& settings = std::get<emberAssetLoader::MaterialAsset::ForwardSettings>(materialAsset.renderModeSettings);
				return CreateForwardMaterial(settings.renderMode, materialShader, materialAsset.materialName);
			}
			default:
				throw std::runtime_error("MaterialManager::CreateMaterial(...) failed. Unsupported material pass.");
		}
	}
	GizmoMaterial MaterialManager::CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateGizmoMaterialShader(vertexSpv, fragmentSpv, name);
		return CreateGizmoMaterial(renderMode, materialShader, name);
	}
	GizmoMaterial MaterialManager::CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CreateGizmoMaterial(materialShader.m_materialShaderId, renderMode, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? GizmoMaterial() : GizmoMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::CreateShadowMaterial(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateShadowMaterialShader(vertexSpv, name);
		return CreateShadowMaterial(materialShader, name);
	}
	ShadowMaterial MaterialManager::CreateShadowMaterial(const MaterialShader& materialShader, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CreateShadowMaterial(materialShader.m_materialShaderId, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? ShadowMaterial() : ShadowMaterial{ materialId };
	}
	DeferredMaterial MaterialManager::CreateDeferredGeometryMaterial(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateDeferredGeometryMaterialShader(vertexSpv, fragmentSpv, name);
		return CreateDeferredGeometryMaterial(materialShader, name);
	}
	DeferredMaterial MaterialManager::CreateDeferredGeometryMaterial(const MaterialShader& materialShader, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CreateDeferredGeometryMaterial(materialShader.m_materialShaderId, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? DeferredMaterial() : DeferredMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateForwardMaterialShader(vertexSpv, fragmentSpv, name);
		return CreateForwardMaterial(renderMode, materialShader, name);
	}
	ForwardMaterial MaterialManager::CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CreateForwardMaterial(materialShader.m_materialShaderId, renderMode, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? ForwardMaterial() : ForwardMaterial{ materialId };
	}



	// Cloners:
	GizmoMaterial MaterialManager::CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneGizmoMaterial(sourceMaterial.m_materialId, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? GizmoMaterial() : GizmoMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneGizmoMaterial(sourceMaterial.m_materialId, renderMode, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? GizmoMaterial() : GizmoMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterialWithDefaultBindings(const GizmoMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneGizmoMaterialWithDefaultBindings(sourceMaterial.m_materialId, name);
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
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneForwardMaterial(sourceMaterial.m_materialId, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? ForwardMaterial() : ForwardMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CloneForwardMaterial(const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneForwardMaterial(sourceMaterial.m_materialId, renderMode, name);
		return materialId.index == emberCommon::invalidMaterialId.index ? ForwardMaterial() : ForwardMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CloneForwardMaterialWithDefaultBindings(const ForwardMaterial& sourceMaterial, const std::string& name)
	{
		emberCommon::MaterialId materialId = s_pIMaterialManager->CloneForwardMaterialWithDefaultBindings(sourceMaterial.m_materialId, name);
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
			return Material();
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
		if (Renderer::s_pIRenderer == nullptr)
			throw std::runtime_error("MaterialManager::Init() failed. Renderer is not initialized.");

		s_pIMaterialManager.reset(Renderer::s_pIRenderer->CreateMaterialManager(MaterialShaderManager::GetInterfaceHandle()));
		if (s_pIMaterialManager == nullptr)
			throw std::runtime_error("MaterialManager::Init() failed. Renderer returned a nullptr material manager.");

		const std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "materialAssets").make_preferred();
    	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directoryPath))
    	{
			// Skip invalid files:
    	    if (!entry.is_regular_file())
    	        continue;
    	    if (entry.path().extension() != ".json")
    	        continue;

    	    const std::filesystem::path& jsonPath = entry.path();
			emberAssetLoader::MaterialAsset materialAsset = emberAssetLoader::MaterialAssetLoader::Load(jsonPath);
			CreateMaterial(materialAsset);
    	}
	}
	void MaterialManager::Clear()
	{
		s_pIMaterialManager.reset();
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
	const emberCommon::MaterialShaderId* MaterialManager::TryGetMaterialShaderId(emberCommon::MaterialId materialId)
	{
		return s_pIMaterialManager->TryGetMaterialShaderId(materialId);
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