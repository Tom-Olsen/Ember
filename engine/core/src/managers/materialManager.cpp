#include "materialManager.h"
#include "commonRenderQueue.h"
#include "iMaterial.h"
#include "logger.h"
#include "material.h"
#include "renderer.h"



namespace emberEngine
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

		uint32_t opaqueQueue = emberCommon::RenderQueue::opaque;
		uint32_t skyboxQueue = emberCommon::RenderQueue::skybox;
		uint32_t transparentQueue = emberCommon::RenderQueue::transparent;
		emberCommon::RenderMode opaqueMode = emberCommon::RenderMode::opaque;
		emberCommon::RenderMode transparentMode = emberCommon::RenderMode::transparent;
		emberCommon::RenderMode skyboxMode = emberCommon::RenderMode::skybox;

		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
		//Material* testMaterial = new Material(Material::Type::forward, "testMaterial", directoryPath + "/test.vert.spv", directoryPath + "/test.frag.spv");
		//AddMaterial(testMaterial);

		CreateMaterial(opaqueMode, "errorMaterial", opaqueQueue, directoryPath / "error.vert.spv", directoryPath / "error.frag.spv");
		CreateMaterial(opaqueMode, "defaultMaterial", opaqueQueue, directoryPath / "default.vert.spv", directoryPath / "default.frag.spv");
		CreateMaterial(transparentMode, "transparentMaterial", transparentQueue, directoryPath / "transparent.vert.spv", directoryPath / "transparent.frag.spv");
		CreateMaterial(opaqueMode, "presentMaterial", opaqueQueue, directoryPath / "present.vert.spv", directoryPath / "present.frag.spv");
		CreateMaterial(opaqueMode, "vertexColorLitMaterial", opaqueQueue, directoryPath / "vertexColorLit.vert.spv", directoryPath / "vertexColorLit.frag.spv");
		CreateMaterial(opaqueMode, "vertexColorUnlitMaterial", opaqueQueue, directoryPath / "vertexColorUnlit.vert.spv", directoryPath / "vertexColorUnlit.frag.spv");
		CreateMaterial(opaqueMode, "normalMaterial", opaqueQueue, directoryPath / "normals.vert.spv", directoryPath / "normals.frag.spv");
		CreateMaterial(skyboxMode, "skyboxMaterial", skyboxQueue, directoryPath / "skybox.vert.spv", directoryPath / "skybox.frag.spv");
		CreateMaterial(opaqueMode, "simpleLitMaterial", opaqueQueue, directoryPath / "simpleLit.vert.spv", directoryPath / "simpleLit.frag.spv");
		CreateMaterial(opaqueMode, "simpleUnlitMaterial", opaqueQueue, directoryPath / "simpleUnlit.vert.spv", directoryPath / "simpleUnlit.frag.spv");
		
		// For testing spirv reflect:
		CreateMaterial(opaqueMode, "testMaterial", opaqueQueue, directoryPath / "test.vert.spv", directoryPath / "test.frag.spv");

	}
	void MaterialManager::Clear()
	{
		s_materialInterfacesMap.clear();
		s_isInitialized = false;
	}



	// Add/Get/Delete:
	Material MaterialManager::CreateMaterial(emberCommon::RenderMode renderMode, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it != s_materialInterfacesMap.end())
		{
			LOG_WARN("Material '{}' already exists — returning existing instance.", name);
			return Material{ it->second.get() };
		}

		emberBackendInterface::IMaterial* pIMaterial = Renderer::CreateMaterial(renderMode, name, renderQueue, vertexSpv, fragmentSpv);
		auto result = s_materialInterfacesMap.emplace(name, std::unique_ptr<emberBackendInterface::IMaterial>(pIMaterial));
		return Material{ result.first->second.get() };
	}
	Material MaterialManager::GetMaterial(const std::string& name)
	{
		auto it = s_materialInterfacesMap.find(name);
		if (it == s_materialInterfacesMap.end())
			throw std::runtime_error("Material not found: " + name);
		return Material{ it->second.get() };
	}
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