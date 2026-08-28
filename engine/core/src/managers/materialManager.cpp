#include "materialManager.h"
#include "iMaterial.h"
#include "logger.h"
#include "materialShader.h"
#include "materialShaderManager.h"
#include "renderer.h"
#include <stdexcept>



namespace emberCore
{
	// Static members:
	bool MaterialManager::s_isInitialized = false;
	MaterialId MaterialManager::s_defaultShadowMaterialId = invalidMaterialId;
	std::unordered_map<std::string, uint32_t> MaterialManager::s_materialIdsMap;
	std::vector<MaterialManager::MaterialSlot> MaterialManager::s_materialSlots;
	std::vector<uint32_t> MaterialManager::s_freeMaterialIds;



	// Initialization/Cleanup:
	void MaterialManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		// Helper variables:
		const emberCommon::ForwardRenderMode forwardOpaqueMode = emberCommon::ForwardRenderMode::opaque;
		const emberCommon::ForwardRenderMode forwardTransparentMode = emberCommon::ForwardRenderMode::transparent;
		const emberCommon::ForwardRenderMode forwardSkyboxMode = emberCommon::ForwardRenderMode::skybox;
		const emberCommon::GizmoRenderMode gizmoOpaqueMode = emberCommon::GizmoRenderMode::opaque;
		const emberCommon::GizmoRenderMode gizmoTransparentMode = emberCommon::GizmoRenderMode::transparent;
		const std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();

		// Create MaterialShaders:
		MaterialShader outlineShader = MaterialShaderManager::CreateOutlineMaterialShader(directoryPath / "outline.vert.spv", directoryPath / "outline.frag.spv", "outlineShader");
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
		MaterialShader defaultShadowShader = MaterialShaderManager::CreateShadowMaterialShader(directoryPath / "shadow.vert.spv", "defaultShadowShader");
		MaterialShader deferredLightingShader = MaterialShaderManager::CreateDeferredLightingMaterialShader(directoryPath / "deferredLighting.vert.spv", directoryPath / "deferredLighting.frag.spv", "deferredLightingShader");
		MaterialShader presentShader = MaterialShaderManager::CreatePresentMaterialShader(directoryPath / "present.vert.spv", directoryPath / "present.frag.spv", "presentShader");

		// Create Materials:
		ShadowMaterial defaultShadowMaterial = CreateShadowMaterial(defaultShadowShader, "defaultShadowMaterial", MaterialRole::accessible);
		s_defaultShadowMaterialId = defaultShadowMaterial.m_materialId;
		Material deferredLightingMaterial = CreateDeferredLightingMaterial(deferredLightingShader, "deferredLightingMaterial", MaterialRole::immutable);
		Material outlineMaterial = CreateOutlineMaterial(outlineShader, "outlineMaterial", MaterialRole::immutable);
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
		Material presentMaterial = CreatePresentMaterial(presentShader, "presentMaterial", MaterialRole::immutable);
		Renderer::SetDefaultMaterials(outlineMaterial, defaultShadowMaterial, deferredLightingMaterial, presentMaterial);
	}
	void MaterialManager::Clear()
	{
		if (!s_isInitialized)
			return;
		Renderer::ClearDefaultMaterials();
		for (uint32_t id = 0; id < s_materialSlots.size(); id++)
		{
			MaterialSlot& slot = s_materialSlots[id];
			if (slot.managedMaterial.pIMaterial == nullptr)
				continue;

			Renderer::DestroyMaterial(slot.managedMaterial.pIMaterial.release());
			slot.managedMaterial.name.clear();
			slot.managedMaterial.roles = MaterialRole::none;
			slot.managedMaterial.materialShaderId = invalidMaterialShaderId;
			slot.managedMaterial.shadowMaterialId = invalidMaterialId;
			slot.generation++;
			s_freeMaterialIds.push_back(id);
		}
		s_materialIdsMap.clear();
		s_defaultShadowMaterialId = invalidMaterialId;
		s_isInitialized = false;
	}



	// Creators:
	GizmoMaterial MaterialManager::CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateGizmoMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("MaterialManager::CreateGizmoMaterial(...) failed. MaterialShader is not a gizmo shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return GizmoMaterial();
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateGizmoMaterial(renderMode, pIMaterialShader, name);
		AddMaterial(name, MaterialRole::defaultRole, materialShader.m_materialShaderId, pIMaterial, materialId);
		return GizmoMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::CreateShadowMaterial(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateShadowMaterialShader(vertexSpv, name);
		return CreateShadowMaterial(materialShader, name);
	}
	ShadowMaterial MaterialManager::CreateShadowMaterial(const MaterialShader& materialShader, const std::string& name)
	{
		return CreateShadowMaterial(materialShader, name, MaterialRole::defaultRole);
	}
	DeferredMaterial MaterialManager::CreateDeferredGeometryMaterial(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateDeferredGeometryMaterialShader(vertexSpv, fragmentSpv, name);
		return CreateDeferredGeometryMaterial(materialShader, name);
	}
	DeferredMaterial MaterialManager::CreateDeferredGeometryMaterial(const MaterialShader& materialShader, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateDeferredGeometryMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
			throw std::runtime_error("MaterialManager::CreateDeferredGeometryMaterial(...) failed. MaterialShader is not a deferred geometry shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return DeferredMaterial();
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateDeferredGeometryMaterial(pIMaterialShader, name);
		AddMaterial(name, MaterialRole::defaultRole, materialShader.m_materialShaderId, pIMaterial, materialId);
		return DeferredMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateForwardMaterialShader(vertexSpv, fragmentSpv, name);
		return CreateForwardMaterial(renderMode, materialShader, name);
	}
	ForwardMaterial MaterialManager::CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const MaterialShader& materialShader, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateForwardMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("MaterialManager::CreateForwardMaterial(...) failed. MaterialShader is not a forward shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return ForwardMaterial();
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateForwardMaterial(renderMode, pIMaterialShader, name);
		AddMaterial(name, MaterialRole::defaultRole, materialShader.m_materialShaderId, pIMaterial, materialId);
		return ForwardMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateGizmoMaterialShader(vertexSpv, fragmentSpv, name);
		return CreateGizmoMaterial(renderMode, materialShader, name);
	}



	// Cloners:
	GizmoMaterial MaterialManager::CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return GizmoMaterial();
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material shader is invalid.");

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CloneGizmoMaterial(sourceMaterial.GetInterfaceHandle(), name);
		AddMaterial(name, MaterialRole::defaultRole, *pMaterialShaderId, pIMaterial, materialId);
		return GizmoMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return GizmoMaterial();
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material shader is invalid.");

		std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial(Renderer::CloneGizmoMaterial(sourceMaterial.GetInterfaceHandle(), name));
		pIMaterial->SetGizmoRenderMode(renderMode);
		AddMaterial(name, MaterialRole::defaultRole, *pMaterialShaderId, pIMaterial.release(), materialId);
		return GizmoMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::CloneShadowMaterial(const ShadowMaterial& sourceMaterial, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return ShadowMaterial();
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneShadowMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneShadowMaterial(...) failed. Source material shader is invalid.");

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CloneShadowMaterial(sourceMaterial.GetInterfaceHandle(), name);
		AddMaterial(name, MaterialRole::defaultRole, *pMaterialShaderId, pIMaterial, materialId);
		return ShadowMaterial{ materialId };
	}
	DeferredMaterial MaterialManager::CloneDeferredGeometryMaterial(const DeferredMaterial& sourceMaterial, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return DeferredMaterial();
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneDeferredGeometryMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneDeferredGeometryMaterial(...) failed. Source material shader is invalid.");

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CloneDeferredGeometryMaterial(sourceMaterial.GetInterfaceHandle(), name);
		AddMaterial(name, MaterialRole::defaultRole, *pMaterialShaderId, pIMaterial, materialId);
		MaterialId shadowMaterialId = GetShadowMaterialIdForSurfaceMaterial(sourceMaterial.m_materialId);
		if (shadowMaterialId.index != invalidMaterialId.index)
			SetShadowMaterial(materialId, shadowMaterialId);
		return DeferredMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CloneForwardMaterial(const ForwardMaterial& sourceMaterial, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return ForwardMaterial();
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material shader is invalid.");

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CloneForwardMaterial(sourceMaterial.GetInterfaceHandle(), name);
		AddMaterial(name, MaterialRole::defaultRole, *pMaterialShaderId, pIMaterial, materialId);
		MaterialId shadowMaterialId = GetShadowMaterialIdForSurfaceMaterial(sourceMaterial.m_materialId);
		if (shadowMaterialId.index != invalidMaterialId.index)
			SetShadowMaterial(materialId, shadowMaterialId);
		return ForwardMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CloneForwardMaterial(const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return ForwardMaterial();
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material shader is invalid.");

		std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial(Renderer::CloneForwardMaterial(sourceMaterial.GetInterfaceHandle(), name));
		pIMaterial->SetForwardRenderMode(renderMode);
		AddMaterial(name, MaterialRole::defaultRole, *pMaterialShaderId, pIMaterial.release(), materialId);
		MaterialId shadowMaterialId = GetShadowMaterialIdForSurfaceMaterial(sourceMaterial.m_materialId);
		if (shadowMaterialId.index != invalidMaterialId.index)
			SetShadowMaterial(materialId, shadowMaterialId);
		return ForwardMaterial{ materialId };
	}



	// Getters:
	Material MaterialManager::GetMaterial(const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (GetMaterialInterface(materialId) == nullptr || !HasMaterialRole(materialId, MaterialRole::accessible))
		{
			LOG_WARN("MaterialManager::GetMaterial(...) failed. Material '{}' not found or expired.", name);
			return Material();
		}
		return Material{ materialId };
	}
	GizmoMaterial MaterialManager::GetGizmoMaterial(const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		emberBackendInterface::IMaterial* pIMaterial = GetMaterialInterface(materialId);
		if (pIMaterial == nullptr || !HasMaterialRole(materialId, MaterialRole::accessible))
		{
			LOG_WARN("MaterialManager::GetGizmoMaterial(...) failed. Material '{}' not found or expired.", name);
			return GizmoMaterial();
		}
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
		{
			LOG_WARN("MaterialManager::GetGizmoMaterial(...) failed. Material '{}' is not a gizmo material.", name);
			return GizmoMaterial();
		}
		return GizmoMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::GetShadowMaterial(const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		emberBackendInterface::IMaterial* pIMaterial = GetMaterialInterface(materialId);
		if (pIMaterial == nullptr || !HasMaterialRole(materialId, MaterialRole::accessible))
		{
			LOG_WARN("MaterialManager::GetShadowMaterial(...) failed. Material '{}' not found or expired.", name);
			return ShadowMaterial();
		}
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
		{
			LOG_WARN("MaterialManager::GetShadowMaterial(...) failed. Material '{}' is not a shadow material.", name);
			return ShadowMaterial();
		}
		return ShadowMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::GetDefaultShadowMaterial()
	{
		if (GetMaterialInterface(s_defaultShadowMaterialId) == nullptr)
		{
			LOG_WARN("MaterialManager::GetDefaultShadowMaterial() failed. Default shadow material is not initialized.");
			return ShadowMaterial();
		}
		return ShadowMaterial{ s_defaultShadowMaterialId };
	}
	DeferredMaterial MaterialManager::GetDeferredMaterial(const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		emberBackendInterface::IMaterial* pIMaterial = GetMaterialInterface(materialId);
		if (pIMaterial == nullptr || !HasMaterialRole(materialId, MaterialRole::accessible))
		{
			LOG_WARN("MaterialManager::GetDeferredMaterial(...) failed. Material '{}' not found or expired.", name);
			return DeferredMaterial();
		}
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
		{
			LOG_WARN("MaterialManager::GetDeferredMaterial(...) failed. Material '{}' is not a deferred material.", name);
			return DeferredMaterial();
		}
		return DeferredMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::GetForwardMaterial(const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		emberBackendInterface::IMaterial* pIMaterial = GetMaterialInterface(materialId);
		if (pIMaterial == nullptr || !HasMaterialRole(materialId, MaterialRole::accessible))
		{
			LOG_WARN("MaterialManager::GetForwardMaterial(...) failed. Material '{}' not found or expired.", name);
			return ForwardMaterial();
		}
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::forward)
		{
			LOG_WARN("MaterialManager::GetForwardMaterial(...) failed. Material '{}' is not a forward material.", name);
			return ForwardMaterial();
		}
		return ForwardMaterial{ materialId };
	}



	// Deleter:
	void MaterialManager::DeleteMaterial(const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (!HasMaterialRole(materialId, MaterialRole::accessible))
		{
			LOG_WARN("MaterialManager::DeleteMaterial(...) failed. Material '{}' not found or expired.", name);
			return;
		}
		DeleteMaterial(materialId);
	}



	// Debugging:
	void MaterialManager::Print()
	{
		LOG_TRACE("MaterialManager contents:");
		for (const auto& [name, materialId] : s_materialIdsMap)
		{
			const MaterialSlot& slot = s_materialSlots[materialId];
			LOG_TRACE("  {}: id {}, generation {}, roles {}", name, materialId, slot.generation, MaterialRoleToString(slot.managedMaterial.roles));
		}
	}



	// Private methods:
	// Creators:
	Material MaterialManager::CreateOutlineMaterial(const MaterialShader& materialShader, const std::string& name, MaterialRole roles)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateOutlineMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::outline)
			throw std::runtime_error("MaterialManager::CreateOutlineMaterial(...) failed. MaterialShader is not an outline shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return Material();
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateOutlineMaterial(pIMaterialShader, name);
		AddMaterial(name, roles, materialShader.m_materialShaderId, pIMaterial, materialId);
		return Material{ materialId };
	}
	ShadowMaterial MaterialManager::CreateShadowMaterial(const MaterialShader& materialShader, const std::string& name, MaterialRole roles)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateShadowMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::CreateShadowMaterial(...) failed. MaterialShader is not a shadow shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return ShadowMaterial();
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateShadowMaterial(pIMaterialShader, name);
		AddMaterial(name, roles, materialShader.m_materialShaderId, pIMaterial, materialId);
		return ShadowMaterial{ materialId };
	}
	Material MaterialManager::CreateDeferredLightingMaterial(const MaterialShader& materialShader, const std::string& name, MaterialRole roles)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateDeferredLightingMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::deferredLighting)
			throw std::runtime_error("MaterialManager::CreateDeferredLightingMaterial(...) failed. MaterialShader is not a deferred lighting shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return Material();
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateDeferredLightingMaterial(pIMaterialShader, name);
		AddMaterial(name, roles, materialShader.m_materialShaderId, pIMaterial, materialId);
		return Material{ materialId };
	}
	Material MaterialManager::CreatePresentMaterial(const MaterialShader& materialShader, const std::string& name, MaterialRole roles)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreatePresentMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::present)
			throw std::runtime_error("MaterialManager::CreatePresentMaterial(...) failed. MaterialShader is not a present shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists, returning invalid handle.", name);
			return Material();
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreatePresentMaterial(pIMaterialShader, name);
		AddMaterial(name, roles, materialShader.m_materialShaderId, pIMaterial, materialId);
		return Material{ materialId };
	}



	// Getters:
	Material MaterialManager::GetMaterial(MaterialId materialId)
	{
		if (GetMaterialInterface(materialId) == nullptr)
			throw std::runtime_error("MaterialManager::GetMaterial(...) failed. Material is invalid or expired.");
		return Material{ materialId };
	}
	GizmoMaterial MaterialManager::GetGizmoMaterial(MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetMaterialInterface(materialId);
		if (pIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::GetGizmoMaterial(...) failed. Material is invalid or expired.");
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::gizmo)
			throw std::runtime_error("MaterialManager::GetGizmoMaterial(...) failed. Material is not a gizmo material.");
		return GizmoMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::GetShadowMaterial(MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetMaterialInterface(materialId);
		if (pIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::GetShadowMaterial(...) failed. Material is invalid or expired.");
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::GetShadowMaterial(...) failed. Material is not a shadow material.");
		return ShadowMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::GetShadowMaterialForSurfaceMaterial(MaterialId surfaceMaterialId)
	{
		emberBackendInterface::IMaterial* pSurfaceMaterial = GetMaterialInterface(surfaceMaterialId);
		if (pSurfaceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::GetShadowMaterialForSurfaceMaterial(...) failed. Surface material is invalid or expired.");
		if (!IsSurfaceMaterialPass(pSurfaceMaterial->GetMaterialPass()))
			throw std::runtime_error("MaterialManager::GetShadowMaterialForSurfaceMaterial(...) failed. Material is not a deferred or forward material.");
		return GetShadowMaterial(GetShadowMaterialIdForSurfaceMaterial(surfaceMaterialId));
	}
	DeferredMaterial MaterialManager::GetDeferredMaterial(MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetMaterialInterface(materialId);
		if (pIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::GetDeferredMaterial(...) failed. Material is invalid or expired.");
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
			throw std::runtime_error("MaterialManager::GetDeferredMaterial(...) failed. Material is not a deferred material.");
		return DeferredMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::GetForwardMaterial(MaterialId materialId)
	{
		emberBackendInterface::IMaterial* pIMaterial = GetMaterialInterface(materialId);
		if (pIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::GetForwardMaterial(...) failed. Material is invalid or expired.");
		if (pIMaterial->GetMaterialPass() != emberCommon::MaterialPass::forward)
			throw std::runtime_error("MaterialManager::GetForwardMaterial(...) failed. Material is not a forward material.");
		return ForwardMaterial{ materialId };
	}
	MaterialId MaterialManager::GetMaterialId(const std::string& name)
	{
		auto it = s_materialIdsMap.find(name);
		if (it == s_materialIdsMap.end())
			return invalidMaterialId;

		const uint32_t materialId = it->second;
		return MaterialId{ materialId, s_materialSlots[materialId].generation };
	}
	emberBackendInterface::IMaterial* MaterialManager::GetMaterialInterface(MaterialId materialId)
	{
		if (materialId.index == invalidMaterialId.index)
			return nullptr;
		if (materialId.index >= s_materialSlots.size())
			return nullptr;

		MaterialSlot& slot = s_materialSlots[materialId.index];
		if (slot.generation != materialId.generation)
			return nullptr;
		return slot.managedMaterial.pIMaterial.get();
	}
	const std::string* MaterialManager::GetMaterialName(MaterialId materialId)
	{
		if (GetMaterialInterface(materialId) == nullptr)
			return nullptr;
		return &s_materialSlots[materialId.index].managedMaterial.name;
	}
	const MaterialShaderId* MaterialManager::GetMaterialShaderId(MaterialId materialId)
	{
		if (GetMaterialInterface(materialId) == nullptr)
			return nullptr;
		return &s_materialSlots[materialId.index].managedMaterial.materialShaderId;
	}
	MaterialId MaterialManager::GetShadowMaterialIdForSurfaceMaterial(MaterialId surfaceMaterialId)
	{
		emberBackendInterface::IMaterial* pSurfaceMaterial = GetMaterialInterface(surfaceMaterialId);
		if (pSurfaceMaterial == nullptr || !IsSurfaceMaterialPass(pSurfaceMaterial->GetMaterialPass()))
			return invalidMaterialId;

		MaterialId& shadowMaterialId = s_materialSlots[surfaceMaterialId.index].managedMaterial.shadowMaterialId;
		emberBackendInterface::IMaterial* pShadowMaterial = GetMaterialInterface(shadowMaterialId);
		if (pShadowMaterial == nullptr || pShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			shadowMaterialId = s_defaultShadowMaterialId;

		pShadowMaterial = GetMaterialInterface(shadowMaterialId);
		if (pShadowMaterial == nullptr || pShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			return invalidMaterialId;
		return shadowMaterialId;
	}



	// Bool checks:
	bool MaterialManager::HasMaterialRole(MaterialId materialId, MaterialRole role)
	{
		if (GetMaterialInterface(materialId) == nullptr)
			return false;
		return emberCore::HasMaterialRole(s_materialSlots[materialId.index].managedMaterial.roles, role);
	}
	bool MaterialManager::MaterialShaderInUse(MaterialShaderId materialShaderId)
	{
		for (const MaterialSlot& slot : s_materialSlots)
			if (slot.managedMaterial.pIMaterial != nullptr && slot.managedMaterial.materialShaderId.index == materialShaderId.index && slot.managedMaterial.materialShaderId.generation == materialShaderId.generation)
				return true;
		return false;
	}
	bool MaterialManager::IsSurfaceMaterialPass(emberCommon::MaterialPass materialPass)
	{
		return materialPass == emberCommon::MaterialPass::deferredGeometry || materialPass == emberCommon::MaterialPass::forward;
	}



	// Add/Delete material:
	void MaterialManager::AddMaterial(const std::string& name, MaterialRole roles, MaterialShaderId materialShaderId, emberBackendInterface::IMaterial* pIMaterial, MaterialId& materialId)
	{
		if (pIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::AddMaterial(...) failed. Backend returned nullptr.");
		MaterialId shadowMaterialId = IsSurfaceMaterialPass(pIMaterial->GetMaterialPass()) ? s_defaultShadowMaterialId : invalidMaterialId;

		if (s_freeMaterialIds.empty())
		{
			if (s_materialSlots.size() >= invalidMaterialId.index)
				throw std::runtime_error("MaterialManager::AddMaterial(...) failed. Material id limit reached.");
			materialId.index = static_cast<uint32_t>(s_materialSlots.size());
			s_materialSlots.push_back({ 1, ManagedMaterial{ name, roles, materialShaderId, shadowMaterialId, std::unique_ptr<emberBackendInterface::IMaterial>(pIMaterial) } });
		}
		else
		{
			materialId.index = s_freeMaterialIds.back();
			s_freeMaterialIds.pop_back();

			MaterialSlot& slot = s_materialSlots[materialId.index];
			slot.managedMaterial.name = name;
			slot.managedMaterial.roles = roles;
			slot.managedMaterial.materialShaderId = materialShaderId;
			slot.managedMaterial.shadowMaterialId = shadowMaterialId;
			slot.managedMaterial.pIMaterial.reset(pIMaterial);
		}

		materialId.generation = s_materialSlots[materialId.index].generation;
		s_materialIdsMap[name] = materialId.index;
	}
	void MaterialManager::ClearShadowMaterial(MaterialId surfaceMaterialId)
	{
		emberBackendInterface::IMaterial* pSurfaceMaterial = GetMaterialInterface(surfaceMaterialId);
		if (pSurfaceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::ClearShadowMaterial(...) failed. Surface material is invalid or expired.");
		if (!IsSurfaceMaterialPass(pSurfaceMaterial->GetMaterialPass()))
			throw std::runtime_error("MaterialManager::ClearShadowMaterial(...) failed. Material is not a deferred or forward material.");

		s_materialSlots[surfaceMaterialId.index].managedMaterial.shadowMaterialId = s_defaultShadowMaterialId;
	}
	void MaterialManager::DeleteMaterial(MaterialId materialId)
	{
		if (GetMaterialInterface(materialId) == nullptr)
			return;
		if (!HasMaterialRole(materialId, MaterialRole::deletable))
		{
			LOG_WARN("MaterialManager::DeleteMaterial(...) failed. Material {} is pinned until MaterialManager::Clear().", s_materialSlots[materialId.index].managedMaterial.name);
			return;
		}

		MaterialSlot& slot = s_materialSlots[materialId.index];
		s_materialIdsMap.erase(slot.managedMaterial.name);
		Renderer::DestroyMaterial(slot.managedMaterial.pIMaterial.release());
		slot.managedMaterial.name.clear();
		slot.managedMaterial.roles = MaterialRole::none;
		slot.managedMaterial.materialShaderId = invalidMaterialShaderId;
		slot.managedMaterial.shadowMaterialId = invalidMaterialId;
		slot.generation++;
		s_freeMaterialIds.push_back(materialId.index);
	}
	void MaterialManager::SetShadowMaterial(MaterialId surfaceMaterialId, MaterialId shadowMaterialId)
	{
		emberBackendInterface::IMaterial* pSurfaceMaterial = GetMaterialInterface(surfaceMaterialId);
		if (pSurfaceMaterial == nullptr)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Surface material is invalid or expired.");
		if (!IsSurfaceMaterialPass(pSurfaceMaterial->GetMaterialPass()))
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Material is not a deferred or forward material.");

		emberBackendInterface::IMaterial* pShadowMaterial = GetMaterialInterface(shadowMaterialId);
		if (pShadowMaterial == nullptr)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Shadow material is invalid or expired.");
		if (pShadowMaterial->GetMaterialPass() != emberCommon::MaterialPass::shadow)
			throw std::runtime_error("MaterialManager::SetShadowMaterial(...) failed. Material is not a shadow material.");

		s_materialSlots[surfaceMaterialId.index].managedMaterial.shadowMaterialId = shadowMaterialId;
	}
}