#include "materialManager.h"
#include "logger.h"
#include "material.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Static members:
	bool MaterialManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<Material>> MaterialManager::s_materials;



	// Initialization and cleanup:
	void MaterialManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		uint32_t opaqueQueue = 1000;
		uint32_t transparentQueue = 2000;
		uint32_t skyboxQueue = 3000;
		Material::Type forwardType = Material::Type::forward;
		Material::Type shadowType = Material::Type::shadow;
		Material::Type skyboxType = Material::Type::skybox;
		Material::Type presentType = Material::Type::present;

		//Material* testMaterial = new Material(Material::Type::forward, "testMaterial", "../src/shaders/test.vert.spv", "../src/shaders/test.frag.spv");
		//AddMaterial(testMaterial);

		Material* pErrorMaterial = new Material(forwardType, "errorMaterial", opaqueQueue, "../src/shaders/error.vert.spv", "../src/shaders/error.frag.spv");
		AddMaterial(pErrorMaterial);

		Material* pDefaultMaterial = new Material(forwardType, "defaultMaterial", opaqueQueue, "../src/shaders/default.vert.spv", "../src/shaders/default.frag.spv");
		AddMaterial(pDefaultMaterial);

		Material* pPresentMaterial = new Material(presentType, "presentMaterial", opaqueQueue, "../src/shaders/present.vert.spv", "../src/shaders/present.frag.spv");
		AddMaterial(pPresentMaterial);

		Material* pVertexColorLitMaterial = new Material(forwardType, "vertexColorLitMaterial", opaqueQueue, "../src/shaders/vertexColorLit.vert.spv", "../src/shaders/vertexColorLit.frag.spv");
		AddMaterial(pVertexColorLitMaterial);

		Material* pVertexColorUnlitMaterial = new Material(forwardType, "vertexColorUnlitMaterial", opaqueQueue, "../src/shaders/vertexColorUnlit.vert.spv", "../src/shaders/vertexColorUnlit.frag.spv");
		AddMaterial(pVertexColorUnlitMaterial);

		Material* pNormalsMaterial = new Material(forwardType, "normalMaterial", opaqueQueue, "../src/shaders/normals.vert.spv", "../src/shaders/normals.frag.spv");
		AddMaterial(pNormalsMaterial);

		Material* pShadowMaterial = new Material(shadowType, "shadowMaterial", opaqueQueue, "../src/shaders/shadow.vert.spv");
		AddMaterial(pShadowMaterial);

		Material* pSkyBoxMaterial = new Material(skyboxType, "skyboxMaterial", skyboxQueue, "../src/shaders/skybox.vert.spv", "../src/shaders/skybox.frag.spv");
		AddMaterial(pSkyBoxMaterial);

		Material* pSimpleLitMaterial = new Material(forwardType, "simpleLitMaterial", opaqueQueue, "../src/shaders/simpleLit.vert.spv", "../src/shaders/simpleLit.frag.spv");
		AddMaterial(pSimpleLitMaterial);

		Material* pSimpleUnlitMaterial = new Material(forwardType, "simpleUnlitMaterial", opaqueQueue, "../src/shaders/simpleUnlit.vert.spv", "../src/shaders/simpleUnlit.frag.spv");
		AddMaterial(pSimpleUnlitMaterial);

		// For testing spirv reflect:
		Material* pTest = new Material(forwardType, "testMaterial", opaqueQueue, "../src/shaders/test.vert.spv", "../src/shaders/test.frag.spv");
		AddMaterial(pTest);

		// For testing the binding missmatch error:
		Material* pTestA = new Material(forwardType, "testAMaterial", opaqueQueue, "../src/shaders/testA.vert.spv", "../src/shaders/testA.frag.spv");
		AddMaterial(pTestA);
		Material* pTestB = new Material(forwardType, "testBMaterial", opaqueQueue, "../src/shaders/testB.vert.spv", "../src/shaders/testB.frag.spv");
		AddMaterial(pTestB);
	}
	void MaterialManager::Clear()
	{
		VulkanContext::WaitDeviceIdle();
		s_materials.clear();
	}



	// Add/get/delete:
	void MaterialManager::AddMaterial(Material* pMaterial)
	{
		// If material already contained in MaterialManager, do nothing.
		if (s_materials.emplace(pMaterial->GetName(), std::unique_ptr<Material>(pMaterial)).second == false)
		{
			LOG_WARN("Material with the name: {} already exists in MaterialManager!", pMaterial->GetName());
			return;
		}
	}
	Material* MaterialManager::GetMaterial(const std::string& name)
	{
		auto it = s_materials.find(name);
		if (it != s_materials.end())
			return it->second.get();
		LOG_WARN("Material '{}' not found!", name);
		return nullptr;
	}
	void MaterialManager::DeleteMaterial(const std::string& name)
	{
		VulkanContext::WaitDeviceIdle();
		s_materials.erase(name);
	}



	// Debugging:
	void MaterialManager::PrintAllMaterialNames()
	{
		LOG_TRACE("Names of all managed materials:");
		for (const auto& pair : s_materials)
			LOG_TRACE(pair.first);
	}
}