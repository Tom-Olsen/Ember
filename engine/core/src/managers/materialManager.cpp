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
		MaterialShader defaultShader = MaterialShaderManager::CreateForwardMaterialShader(directoryPath / "default.vert.spv", directoryPath / "default.frag.spv", "defaultShader");
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
		MaterialShader presentShader = MaterialShaderManager::CreatePresentMaterialShader(directoryPath / "present.vert.spv", directoryPath / "present.frag.spv", "presentShader");

		// Create Materials:
		Material outlineMaterial = CreateOutlineMaterial(outlineShader, "outlineMaterial");
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
		ShadowMaterial defaultShadowMaterial = CreateShadowMaterial(defaultShadowShader, "defaultShadowMaterial");
		Material presentMaterial = CreatePresentMaterial(presentShader, "presentMaterial");
		Renderer::SetDefaultMaterials(outlineMaterial, defaultShadowMaterial, presentMaterial);
	}
	void MaterialManager::Clear()
	{
		for (uint32_t id = 0; id < s_materialSlots.size(); id++)
		{
			MaterialSlot& slot = s_materialSlots[id];
			if (slot.managedMaterial.pIMaterial == nullptr)
				continue;

			Renderer::DestroyMaterial(slot.managedMaterial.pIMaterial.release());
			slot.managedMaterial.name.clear();
			slot.managedMaterial.materialShaderId = invalidMaterialShaderId;
			slot.generation++;
			s_freeMaterialIds.push_back(id);
		}
		s_materialIdsMap.clear();
		s_isInitialized = false;
	}



	// Creators:
	Material MaterialManager::CreateOutlineMaterial(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateOutlineMaterialShader(vertexSpv, fragmentSpv, name);
		return CreateOutlineMaterial(materialShader, name);
	}
	Material MaterialManager::CreateOutlineMaterial(const MaterialShader& materialShader, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateOutlineMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialType() != emberCommon::MaterialType::outline)
			throw std::runtime_error("MaterialManager::CreateOutlineMaterial(...) failed. MaterialShader is not an outline shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return Material{ materialId };
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateOutlineMaterial(pIMaterialShader, name);
		AddMaterial(name, materialShader.m_materialShaderId, pIMaterial, materialId);
		return Material{ materialId };
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
		if (materialShader.GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("MaterialManager::CreateForwardMaterial(...) failed. MaterialShader is not a forward shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return ForwardMaterial{ materialId };
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateForwardMaterial(renderMode, pIMaterialShader, name);
		AddMaterial(name, materialShader.m_materialShaderId, pIMaterial, materialId);
		return ForwardMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateGizmoMaterialShader(vertexSpv, fragmentSpv, name);
		return CreateGizmoMaterial(renderMode, materialShader, name);
	}
	GizmoMaterial MaterialManager::CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const MaterialShader& materialShader, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateGizmoMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("MaterialManager::CreateGizmoMaterial(...) failed. MaterialShader is not a gizmo shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return GizmoMaterial{ materialId };
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateGizmoMaterial(renderMode, pIMaterialShader, name);
		AddMaterial(name, materialShader.m_materialShaderId, pIMaterial, materialId);
		return GizmoMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::CreateShadowMaterial(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreateShadowMaterialShader(vertexSpv, name);
		return CreateShadowMaterial(materialShader, name);
	}
	ShadowMaterial MaterialManager::CreateShadowMaterial(const MaterialShader& materialShader, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreateShadowMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialType() != emberCommon::MaterialType::shadow)
			throw std::runtime_error("MaterialManager::CreateShadowMaterial(...) failed. MaterialShader is not a shadow shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return ShadowMaterial{ materialId };
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateShadowMaterial(pIMaterialShader, name);
		AddMaterial(name, materialShader.m_materialShaderId, pIMaterial, materialId);
		return ShadowMaterial{ materialId };
	}
	Material MaterialManager::CreatePresentMaterial(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShader materialShader = MaterialShaderManager::CreatePresentMaterialShader(vertexSpv, fragmentSpv, name);
		return CreatePresentMaterial(materialShader, name);
	}
	Material MaterialManager::CreatePresentMaterial(const MaterialShader& materialShader, const std::string& name)
	{
		emberBackendInterface::IMaterialShader* pIMaterialShader = materialShader.GetInterfaceHandle();
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialManager::CreatePresentMaterial(...) failed. MaterialShader is invalid or expired.");
		if (materialShader.GetMaterialType() != emberCommon::MaterialType::present)
			throw std::runtime_error("MaterialManager::CreatePresentMaterial(...) failed. MaterialShader is not a present shader.");

		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return Material{ materialId };
		}
		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreatePresentMaterial(pIMaterialShader, name);
		AddMaterial(name, materialShader.m_materialShaderId, pIMaterial, materialId);
		return Material{ materialId };
	}



	// Cloners:
	ForwardMaterial MaterialManager::CloneForwardMaterial(const ForwardMaterial& sourceMaterial, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return ForwardMaterial{ materialId };
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material shader is invalid.");

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CloneForwardMaterial(sourceMaterial.GetInterfaceHandle(), name);
		AddMaterial(name, *pMaterialShaderId, pIMaterial, materialId);
		return ForwardMaterial{ materialId };
	}
	ForwardMaterial MaterialManager::CloneForwardMaterial(const ForwardMaterial& sourceMaterial, emberCommon::ForwardRenderMode renderMode, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return ForwardMaterial{ materialId };
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneForwardMaterial(...) failed. Source material shader is invalid.");

		std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial(Renderer::CloneForwardMaterial(sourceMaterial.GetInterfaceHandle(), name));
		pIMaterial->SetForwardRenderMode(renderMode);
		AddMaterial(name, *pMaterialShaderId, pIMaterial.release(), materialId);
		return ForwardMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return GizmoMaterial{ materialId };
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material shader is invalid.");

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CloneGizmoMaterial(sourceMaterial.GetInterfaceHandle(), name);
		AddMaterial(name, *pMaterialShaderId, pIMaterial, materialId);
		return GizmoMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::CloneGizmoMaterial(const GizmoMaterial& sourceMaterial, emberCommon::GizmoRenderMode renderMode, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return GizmoMaterial{ materialId };
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneGizmoMaterial(...) failed. Source material shader is invalid.");

		std::unique_ptr<emberBackendInterface::IMaterial> pIMaterial(Renderer::CloneGizmoMaterial(sourceMaterial.GetInterfaceHandle(), name));
		pIMaterial->SetGizmoRenderMode(renderMode);
		AddMaterial(name, *pMaterialShaderId, pIMaterial.release(), materialId);
		return GizmoMaterial{ materialId };
	}
	ShadowMaterial MaterialManager::CloneShadowMaterial(const ShadowMaterial& sourceMaterial, const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (materialId.index != invalidMaterialId.index)
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return ShadowMaterial{ materialId };
		}
		if (!sourceMaterial.IsValid())
			throw std::runtime_error("MaterialManager::CloneShadowMaterial(...) failed. Source material is invalid.");
		const MaterialShaderId* pMaterialShaderId = GetMaterialShaderId(sourceMaterial.m_materialId);
		if (pMaterialShaderId == nullptr)
			throw std::runtime_error("MaterialManager::CloneShadowMaterial(...) failed. Source material shader is invalid.");

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CloneShadowMaterial(sourceMaterial.GetInterfaceHandle(), name);
		AddMaterial(name, *pMaterialShaderId, pIMaterial, materialId);
		return ShadowMaterial{ materialId };
	}



	// Getters:
	Material MaterialManager::GetMaterial(const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		if (GetMaterialInterface(materialId) == nullptr)
		{
			LOG_WARN("MaterialManager::GetMaterial(...) failed. Material '{}' not found or expired.", name);
			return Material();
		}
		return Material{ materialId };
	}
	ForwardMaterial MaterialManager::GetForwardMaterial(const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		emberBackendInterface::IMaterial* pIMaterial = GetMaterialInterface(materialId);
		if (pIMaterial == nullptr)
		{
			LOG_WARN("MaterialManager::GetForwardMaterial(...) failed. Material '{}' not found or expired.", name);
			return ForwardMaterial();
		}
		if (pIMaterial->GetMaterialType() != emberCommon::MaterialType::forward)
		{
			LOG_WARN("MaterialManager::GetForwardMaterial(...) failed. Material '{}' is not a forward material.", name);
			return ForwardMaterial();
		}
		return ForwardMaterial{ materialId };
	}
	GizmoMaterial MaterialManager::GetGizmoMaterial(const std::string& name)
	{
		MaterialId materialId = GetMaterialId(name);
		emberBackendInterface::IMaterial* pIMaterial = GetMaterialInterface(materialId);
		if (pIMaterial == nullptr)
		{
			LOG_WARN("MaterialManager::GetGizmoMaterial(...) failed. Material '{}' not found or expired.", name);
			return GizmoMaterial();
		}
		if (pIMaterial->GetMaterialType() != emberCommon::MaterialType::gizmo)
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
		if (pIMaterial == nullptr)
		{
			LOG_WARN("MaterialManager::GetShadowMaterial(...) failed. Material '{}' not found or expired.", name);
			return ShadowMaterial();
		}
		if (pIMaterial->GetMaterialType() != emberCommon::MaterialType::shadow)
		{
			LOG_WARN("MaterialManager::GetShadowMaterial(...) failed. Material '{}' is not a shadow material.", name);
			return ShadowMaterial();
		}
		return ShadowMaterial{ materialId };
	}



	// Deleter:
	void MaterialManager::DeleteMaterial(const std::string& name)
	{
		DeleteMaterial(GetMaterialId(name));
	}



	// Debugging:
	void MaterialManager::Print()
	{
		LOG_TRACE("MaterialManager contents:");
		for (auto& [name, materialId] : s_materialIdsMap)
			LOG_TRACE("  {}: id {}, generation {}", name, materialId, s_materialSlots[materialId].generation);
	}



	// Private methods:
	MaterialId MaterialManager::GetMaterialId(const std::string& name)
	{
		auto it = s_materialIdsMap.find(name);
		if (it == s_materialIdsMap.end())
			return invalidMaterialId;

		const uint32_t materialId = it->second;
		return MaterialId{ materialId, s_materialSlots[materialId].generation };
	}
	MaterialId MaterialManager::GetMaterialId(emberBackendInterface::IMaterial* pIMaterial)
	{
		if (pIMaterial == nullptr)
			return invalidMaterialId;

		for (uint32_t id = 0; id < s_materialSlots.size(); id++)
		{
			MaterialSlot& slot = s_materialSlots[id];
			if (slot.managedMaterial.pIMaterial.get() != pIMaterial)
				continue;

			return MaterialId{ id, slot.generation };
		}

		return invalidMaterialId;
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
	bool MaterialManager::MaterialShaderInUse(MaterialShaderId materialShaderId)
	{
		for (const MaterialSlot& slot : s_materialSlots)
			if (slot.managedMaterial.pIMaterial != nullptr && slot.managedMaterial.materialShaderId.index == materialShaderId.index && slot.managedMaterial.materialShaderId.generation == materialShaderId.generation)
				return true;
		return false;
	}
	void MaterialManager::AddMaterial(const std::string& name, MaterialShaderId materialShaderId, emberBackendInterface::IMaterial* pIMaterial, MaterialId& materialId)
	{
		if (pIMaterial == nullptr)
			throw std::runtime_error("MaterialManager::AddMaterial(...) failed. Backend returned nullptr.");

		if (s_freeMaterialIds.empty())
		{
			if (s_materialSlots.size() >= invalidMaterialId.index)
				throw std::runtime_error("MaterialManager::AddMaterial(...) failed. Material id limit reached.");
			materialId.index = static_cast<uint32_t>(s_materialSlots.size());
			s_materialSlots.push_back({ 1, ManagedMaterial{ name, materialShaderId, std::unique_ptr<emberBackendInterface::IMaterial>(pIMaterial) } });
		}
		else
		{
			materialId.index = s_freeMaterialIds.back();
			s_freeMaterialIds.pop_back();

			MaterialSlot& slot = s_materialSlots[materialId.index];
			slot.managedMaterial.name = name;
			slot.managedMaterial.materialShaderId = materialShaderId;
			slot.managedMaterial.pIMaterial.reset(pIMaterial);
		}

		materialId.generation = s_materialSlots[materialId.index].generation;
		s_materialIdsMap[name] = materialId.index;
	}
	void MaterialManager::DeleteMaterial(MaterialId materialId)
	{
		if (GetMaterialInterface(materialId) == nullptr)
			return;

		MaterialSlot& slot = s_materialSlots[materialId.index];
		s_materialIdsMap.erase(slot.managedMaterial.name);
		Renderer::DestroyMaterial(slot.managedMaterial.pIMaterial.release());
		slot.managedMaterial.name.clear();
		slot.managedMaterial.materialShaderId = invalidMaterialShaderId;
		slot.generation++;
		s_freeMaterialIds.push_back(materialId.index);
	}
}