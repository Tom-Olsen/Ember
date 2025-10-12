#include "materialManager.h"
#include "commonRenderQueue.h"
#include "logger.h"
#include "material.h"
#include <filesystem>



namespace emberEngine
{
	// Static members:
	bool MaterialManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<Material>> MaterialManager::s_materials;



	// Initialization/Cleanup:
	void MaterialManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		uint32_t shadowQueue = emberCommon::RenderQueue::shadow;
		uint32_t opaqueQueue = emberCommon::RenderQueue::opaque;
		uint32_t skyboxQueue = emberCommon::RenderQueue::skybox;
		uint32_t transparentQueue = emberCommon::RenderQueue::transparent;
		emberCommon::MaterialType forwardOpaqueType = emberCommon::MaterialType::forwardOpaque;
		emberCommon::MaterialType forwardTransparentType = emberCommon::MaterialType::forwardTransparent;
		emberCommon::MaterialType shadowType = emberCommon::MaterialType::shadow;
		emberCommon::MaterialType skyboxType = emberCommon::MaterialType::skybox;
		emberCommon::MaterialType presentType = emberCommon::MaterialType::present;

		std::filesystem::path directoryPath = (std::filesystem::path(CORE_SHADERS_DIR) / "bin").make_preferred();
		//Material* testMaterial = new Material(Material::Type::forward, "testMaterial", directoryPath + "/test.vert.spv", directoryPath + "/test.frag.spv");
		//AddMaterial(testMaterial);

		Material errorMaterial(forwardOpaqueType, "errorMaterial", opaqueQueue, directoryPath / "error.vert.spv", directoryPath / "error.frag.spv");
		AddMaterial(std::move(errorMaterial));

		Material defaultMaterial(forwardOpaqueType, "defaultMaterial", opaqueQueue, directoryPath / "default.vert.spv", directoryPath / "default.frag.spv");
		AddMaterial(std::move(defaultMaterial));

		Material transparentMaterial(forwardTransparentType, "transparentMaterial", transparentQueue, directoryPath / "transparent.vert.spv", directoryPath / "transparent.frag.spv");
		AddMaterial(std::move(transparentMaterial));

		Material presentMaterial(presentType, "presentMaterial", opaqueQueue, directoryPath / "present.vert.spv", directoryPath / "present.frag.spv");
		AddMaterial(std::move(presentMaterial));

		Material vertexColorLitMaterial(forwardOpaqueType, "vertexColorLitMaterial", opaqueQueue, directoryPath / "vertexColorLit.vert.spv", directoryPath / "vertexColorLit.frag.spv");
		AddMaterial(std::move(vertexColorLitMaterial));

		Material vertexColorUnlitMaterial(forwardOpaqueType, "vertexColorUnlitMaterial", opaqueQueue, directoryPath / "vertexColorUnlit.vert.spv", directoryPath / "vertexColorUnlit.frag.spv");
		AddMaterial(std::move(vertexColorUnlitMaterial));

		Material normalsMaterial(forwardOpaqueType, "normalMaterial", opaqueQueue, directoryPath / "normals.vert.spv", directoryPath / "normals.frag.spv");
		AddMaterial(std::move(normalsMaterial));

		Material shadowMaterial(shadowType, "shadowMaterial", shadowQueue, directoryPath / "shadow.vert.spv");
		AddMaterial(std::move(shadowMaterial));

		Material skyBoxMaterial(skyboxType, "skyboxMaterial", skyboxQueue, directoryPath / "skybox.vert.spv", directoryPath / "skybox.frag.spv");
		AddMaterial(std::move(skyBoxMaterial));

		Material simpleLitMaterial(forwardOpaqueType, "simpleLitMaterial", opaqueQueue, directoryPath / "simpleLit.vert.spv", directoryPath / "simpleLit.frag.spv");
		AddMaterial(std::move(simpleLitMaterial));

		Material simpleUnlitMaterial(forwardOpaqueType, "simpleUnlitMaterial", opaqueQueue, directoryPath / "simpleUnlit.vert.spv", directoryPath / "simpleUnlit.frag.spv");
		AddMaterial(std::move(simpleUnlitMaterial));

		// For testing spirv reflect:
		Material test(forwardOpaqueType, "testMaterial", opaqueQueue, directoryPath / "test.vert.spv", directoryPath / "test.frag.spv");
		AddMaterial(std::move(test));

		// For testing the binding missmatch error:
		Material testA(forwardOpaqueType, "testAMaterial", opaqueQueue, directoryPath / "testA.vert.spv", directoryPath / "testA.frag.spv");
		AddMaterial(std::move(testA));
		Material testB(forwardOpaqueType, "testBMaterial", opaqueQueue, directoryPath / "testB.vert.spv", directoryPath / "testB.frag.spv");
		AddMaterial(std::move(testB));
	}
	void MaterialManager::Clear()
	{
		s_materials.clear();
		s_isInitialized = false;
	}



	// Add/Get/Delete:
	void MaterialManager::AddMaterial(Material&& material)
	{
		std::string name = material.GetName();
		auto newMaterial = std::make_unique<Material>(std::move(material));
		if (!s_materials.emplace(name, std::move(newMaterial)).second)
			LOG_WARN("Material with the name: {} already exists in MaterialManager!", name);
	}
	Material& MaterialManager::GetMaterial(const std::string& name)
	{
		auto it = s_materials.find(name);
		if (it == s_materials.end())
		{
			LOG_ERROR("Material not found: {}", name);
			return GetMaterial("errorMaterial");
		}
		return *(it->second);
	}
	Material* MaterialManager::TryGetMaterial(const std::string& name)
	{
		auto it = s_materials.find(name);
		if (it != s_materials.end())
			return it->second.get();
		LOG_WARN("Material '{}' not found!", name);
		return TryGetMaterial("errorMaterial");
	}
	void MaterialManager::DeleteMaterial(const std::string& name)
	{
		s_materials.erase(name);
	}



	// Debugging:
	void MaterialManager::Print()
	{
		LOG_TRACE("MaterialManager content:");
		for (const auto& pair : s_materials)
			LOG_TRACE(pair.first);
	}
}