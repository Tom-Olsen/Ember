#include "materialManager.h"
#include "iMaterial.h"
#include "logger.h"
#include "renderer.h"
#include <stdexcept>



namespace emberCore
{
	// Static members:
	bool MaterialManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<emberBackendInterface::IMaterial>> MaterialManager::s_materialInterfacesMap;



	// Initialization/Cleanup:
	void MaterialManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		emberCommon::ForwardRenderMode forwardOpaqueMode = emberCommon::ForwardRenderMode::opaque;
		emberCommon::ForwardRenderMode forwardTransparentMode = emberCommon::ForwardRenderMode::transparent;
		emberCommon::ForwardRenderMode forwardSkyboxMode = emberCommon::ForwardRenderMode::skybox;
		emberCommon::GizmoRenderMode gizmoOpaqueMode = emberCommon::GizmoRenderMode::opaque;
		emberCommon::GizmoRenderMode gizmoTransparentMode = emberCommon::GizmoRenderMode::transparent;

		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
		//Material* testMaterial = new Material(Material::Type::forward, "testMaterial", directoryPath + "/test.vert.spv", directoryPath + "/test.frag.spv");
		//AddMaterial(testMaterial);

		CreateForwardMaterial(forwardOpaqueMode, "errorMaterial", directoryPath / "error.vert.spv", directoryPath / "error.frag.spv");
		CreateForwardMaterial(forwardOpaqueMode, "defaultMaterial", directoryPath / "default.vert.spv", directoryPath / "default.frag.spv");
		CreateForwardMaterial(forwardTransparentMode, "transparentMaterial", directoryPath / "transparent.vert.spv", directoryPath / "transparent.frag.spv");
		CreateForwardMaterial(forwardOpaqueMode, "presentMaterial", directoryPath / "present.vert.spv", directoryPath / "present.frag.spv");
		CreateForwardMaterial(forwardOpaqueMode, "vertexColorLitMaterial", directoryPath / "vertexColorLit.vert.spv", directoryPath / "vertexColorLit.frag.spv");
		CreateForwardMaterial(forwardOpaqueMode, "vertexColorUnlitMaterial", directoryPath / "vertexColorUnlit.vert.spv", directoryPath / "vertexColorUnlit.frag.spv");
		CreateForwardMaterial(forwardOpaqueMode, "normalMaterial", directoryPath / "normals.vert.spv", directoryPath / "normals.frag.spv");
		CreateForwardMaterial(forwardSkyboxMode, "skyboxMaterial", directoryPath / "skybox.vert.spv", directoryPath / "skybox.frag.spv");
		CreateForwardMaterial(forwardOpaqueMode, "simpleLitMaterial", directoryPath / "simpleLit.vert.spv", directoryPath / "simpleLit.frag.spv");
		CreateForwardMaterial(forwardOpaqueMode, "simpleUnlitMaterial", directoryPath / "simpleUnlit.vert.spv", directoryPath / "simpleUnlit.frag.spv");
		CreateGizmoMaterial(gizmoOpaqueMode, "gizmoUnlitMaterial", directoryPath / "gizmoUnlit.vert.spv", directoryPath / "gizmoUnlit.frag.spv");
		CreateGizmoMaterial(gizmoOpaqueMode, "gizmoLitMaterial", directoryPath / "gizmoLit.vert.spv", directoryPath / "gizmoLit.frag.spv");
		CreateGizmoMaterial(gizmoOpaqueMode, "gizmoVertexColorUnlitMaterial", directoryPath / "gizmoVertexColorUnlit.vert.spv", directoryPath / "gizmoVertexColorUnlit.frag.spv");
		CreateGizmoMaterial(gizmoOpaqueMode, "gizmoVertexColorLitMaterial", directoryPath / "gizmoVertexColorLit.vert.spv", directoryPath / "gizmoVertexColorLit.frag.spv");
		CreateGizmoMaterial(gizmoTransparentMode, "gizmoUnlitTransparentMaterial", directoryPath / "gizmoUnlit.vert.spv", directoryPath / "gizmoUnlit.frag.spv");
		CreateGizmoMaterial(gizmoTransparentMode, "gizmoLitTransparentMaterial", directoryPath / "gizmoLit.vert.spv", directoryPath / "gizmoLit.frag.spv");
	}
	void MaterialManager::Clear()
	{
		s_materialInterfacesMap.clear();
		s_isInitialized = false;
	}



	// Creators:
	ForwardMaterial MaterialManager::CreateForwardMaterial(emberCommon::ForwardRenderMode renderMode, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it != s_materialInterfacesMap.end())
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return ForwardMaterial{ it->second.get() };
		}

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateForwardMaterial(renderMode, name, vertexSpv, fragmentSpv);
		auto result = s_materialInterfacesMap.emplace(name, std::unique_ptr<emberBackendInterface::IMaterial>(pIMaterial));
		return ForwardMaterial{ result.first->second.get() };
	}
	GizmoMaterial MaterialManager::CreateGizmoMaterial(emberCommon::GizmoRenderMode renderMode, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it != s_materialInterfacesMap.end())
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return GizmoMaterial{ it->second.get() };
		}

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateGizmoMaterial(renderMode, name, vertexSpv, fragmentSpv);
		auto result = s_materialInterfacesMap.emplace(name, std::unique_ptr<emberBackendInterface::IMaterial>(pIMaterial));
		return GizmoMaterial{ result.first->second.get() };
	}
	ShadowMaterial MaterialManager::CreateShadowMaterial(const std::string& name, const std::filesystem::path& vertexSpv)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it != s_materialInterfacesMap.end())
		{
			LOG_WARN("Material '{}' already exists - returning existing instance.", name);
			return ShadowMaterial{ it->second.get() };
		}

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateShadowMaterial(name, vertexSpv);
		auto result = s_materialInterfacesMap.emplace(name, std::unique_ptr<emberBackendInterface::IMaterial>(pIMaterial));
		return ShadowMaterial{ result.first->second.get() };
	}



	// Getters: (throw on fail)
	Material MaterialManager::GetMaterial(const std::string& name)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it == s_materialInterfacesMap.end())
			throw std::runtime_error("Material not found: " + name);
		return Material{ it->second.get() };
	}
	ForwardMaterial MaterialManager::GetForwardMaterial(const std::string& name)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it == s_materialInterfacesMap.end())
			throw std::runtime_error("Forward material not found: " + name);
		if (it->second->GetMaterialType() != emberCommon::MaterialType::forward)
			throw std::runtime_error("Material is not a forward material: " + name);
		return ForwardMaterial{ it->second.get() };
	}
	GizmoMaterial MaterialManager::GetGizmoMaterial(const std::string& name)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it == s_materialInterfacesMap.end())
			throw std::runtime_error("Gizmo material not found: " + name);
		if (it->second->GetMaterialType() != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Material is not a gizmo material: " + name);
		return GizmoMaterial{ it->second.get() };
	}
	ShadowMaterial MaterialManager::GetShadowMaterial(const std::string& name)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it == s_materialInterfacesMap.end())
			throw std::runtime_error("Shadow material not found: " + name);
		if (it->second->GetMaterialType() != emberCommon::MaterialType::shadow)
			throw std::runtime_error("Material is not a shadow material: " + name);
		return ShadowMaterial{ it->second.get() };
	}



	// Try getters: (return invalid Material on fail)
	Material MaterialManager::TryGetMaterial(const std::string& name)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it == s_materialInterfacesMap.end())
		{
			LOG_ERROR("Material '{}' not found!", name);
			return Material();
		}
		return Material{ it->second.get() };
	}
	ForwardMaterial MaterialManager::TryGetForwardMaterial(const std::string& name)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it == s_materialInterfacesMap.end())
		{
			LOG_ERROR("Forward material '{}' not found!", name);
			return ForwardMaterial();
		}
		if (it->second->GetMaterialType() != emberCommon::MaterialType::forward)
		{
			LOG_ERROR("Material '{}' is not a forward material!", name);
			return ForwardMaterial();
		}
		return ForwardMaterial{ it->second.get() };
	}
	GizmoMaterial MaterialManager::TryGetGizmoMaterial(const std::string& name)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it == s_materialInterfacesMap.end())
		{
			LOG_ERROR("Gizmo material '{}' not found!", name);
			return GizmoMaterial();
		}
		if (it->second->GetMaterialType() != emberCommon::MaterialType::gizmo)
		{
			LOG_ERROR("Material '{}' is not a gizmo material!", name);
			return GizmoMaterial();
		}
		return GizmoMaterial{ it->second.get() };
	}
	ShadowMaterial MaterialManager::TryGetShadowMaterial(const std::string& name)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it == s_materialInterfacesMap.end())
		{
			LOG_ERROR("Shadow material '{}' not found!", name);
			return ShadowMaterial();
		}
		if (it->second->GetMaterialType() != emberCommon::MaterialType::shadow)
		{
			LOG_ERROR("Material '{}' is not a shadow material!", name);
			return ShadowMaterial();
		}
		return ShadowMaterial{ it->second.get() };
	}



	// Deleter:
	void MaterialManager::DeleteMaterial(const std::string& name)
	{
		s_materialInterfacesMap.erase(name);
	}




	// Debugging:
	void MaterialManager::Print()
    {
        LOG_TRACE("MaterialManager contents:");
        for (auto& [name, _] : s_materialInterfacesMap)
            LOG_TRACE("  {}", name);
    }
}