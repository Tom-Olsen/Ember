#include "materialManager.h"
#include "iMaterial.h"
#include "iMaterialManager.h"
#include "iRenderer.h"
#include "logger.h"
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

		const emberCommon::ForwardRenderMode forwardOpaqueMode = emberCommon::ForwardRenderMode::opaque;
		const emberCommon::ForwardRenderMode forwardTransparentMode = emberCommon::ForwardRenderMode::transparent;
		const emberCommon::ForwardRenderMode forwardSkyboxMode = emberCommon::ForwardRenderMode::skybox;
		const emberCommon::GizmoRenderMode gizmoOpaqueMode = emberCommon::GizmoRenderMode::opaque;
		const emberCommon::GizmoRenderMode gizmoTransparentMode = emberCommon::GizmoRenderMode::transparent;
		const std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();

		MaterialShader errorShader = MaterialShaderManager::CreateForwardMaterialShader(directoryPath / "error.vert.spv", directoryPath / "error.frag.spv", "errorShader");
		MaterialShader defaultShader = MaterialShaderManager::CreateForwardMaterialShader(directoryPath / "defaultForward.vert.spv", directoryPath / "defaultForward.frag.spv", "defaultShader");
		MaterialShader transparentShader = MaterialShaderManager::CreateForwardMaterialShader(directoryPath / "transparent.vert.spv", directoryPath / "transparent.frag.spv", "transparentShader");
		MaterialShader vertexColorLitShader = MaterialShaderManager::CreateForwardMaterialShader(directoryPath / "vertexColorLit.vert.spv", directoryPath / "vertexColorLit.frag.spv", "vertexColorLitShader");
		MaterialShader vertexColorUnlitShader = MaterialShaderManager::CreateForwardMaterialShader(directoryPath / "vertexColorUnlit.vert.spv", directoryPath / "vertexColorUnlit.frag.spv", "vertexColorUnlitShader");
		MaterialShader normalShader = MaterialShaderManager::CreateForwardMaterialShader(directoryPath / "normals.vert.spv", directoryPath / "normals.frag.spv", "normalShader");
		MaterialShader skyboxShader = MaterialShaderManager::CreateForwardMaterialShader(directoryPath / "skybox.vert.spv", directoryPath / "skybox.frag.spv", "skyboxShader");
		MaterialShader simpleLitShader = MaterialShaderManager::CreateForwardMaterialShader(directoryPath / "simpleLit.vert.spv", directoryPath / "simpleLit.frag.spv", "simpleLitShader");
		MaterialShader simpleUnlitShader = MaterialShaderManager::CreateForwardMaterialShader(directoryPath / "simpleUnlit.vert.spv", directoryPath / "simpleUnlit.frag.spv", "simpleUnlitShader");
		MaterialShader gizmoUnlitShader = MaterialShaderManager::CreateGizmoMaterialShader(directoryPath / "gizmoUnlit.vert.spv", directoryPath / "gizmoUnlit.frag.spv", "gizmoUnlitShader");
		MaterialShader gizmoLitShader = MaterialShaderManager::CreateGizmoMaterialShader(directoryPath / "gizmoLit.vert.spv", directoryPath / "gizmoLit.frag.spv", "gizmoLitShader");
		MaterialShader gizmoVertexColorUnlitShader = MaterialShaderManager::CreateGizmoMaterialShader(directoryPath / "gizmoVertexColorUnlit.vert.spv", directoryPath / "gizmoVertexColorUnlit.frag.spv", "gizmoVertexColorUnlitShader");
		MaterialShader gizmoVertexColorLitShader = MaterialShaderManager::CreateGizmoMaterialShader(directoryPath / "gizmoVertexColorLit.vert.spv", directoryPath / "gizmoVertexColorLit.frag.spv", "gizmoVertexColorLitShader");
		MaterialShader deferredGeometryShader = MaterialShaderManager::CreateDeferredGeometryMaterialShader(directoryPath / "deferredGeometry.vert.spv", directoryPath / "deferredGeometry.frag.spv", "deferredGeometryShader");

		CreateForwardMaterial(forwardOpaqueMode, errorShader, "errorMaterial");
		CreateForwardMaterial(forwardOpaqueMode, defaultShader, "defaultMaterial");
		CreateForwardMaterial(forwardTransparentMode, transparentShader, "transparentMaterial");
		CreateForwardMaterial(forwardOpaqueMode, vertexColorLitShader, "vertexColorLitMaterial");
		CreateForwardMaterial(forwardOpaqueMode, vertexColorUnlitShader, "vertexColorUnlitMaterial");
		CreateForwardMaterial(forwardOpaqueMode, normalShader, "normalMaterial");
		CreateForwardMaterial(forwardSkyboxMode, skyboxShader, "skyboxMaterial");
		CreateForwardMaterial(forwardOpaqueMode, simpleLitShader, "simpleLitMaterial");
		CreateForwardMaterial(forwardOpaqueMode, simpleUnlitShader, "simpleUnlitMaterial");
		CreateGizmoMaterial(gizmoOpaqueMode, gizmoUnlitShader, "gizmoUnlitMaterial");
		CreateGizmoMaterial(gizmoOpaqueMode, gizmoLitShader, "gizmoLitMaterial");
		CreateGizmoMaterial(gizmoOpaqueMode, gizmoVertexColorUnlitShader, "gizmoVertexColorUnlitMaterial");
		CreateGizmoMaterial(gizmoOpaqueMode, gizmoVertexColorLitShader, "gizmoVertexColorLitMaterial");
		CreateGizmoMaterial(gizmoTransparentMode, gizmoUnlitShader, "gizmoUnlitTransparentMaterial");
		CreateGizmoMaterial(gizmoTransparentMode, gizmoLitShader, "gizmoLitTransparentMaterial");
		CreateDeferredGeometryMaterial(deferredGeometryShader, "deferredGeometryMaterial");
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