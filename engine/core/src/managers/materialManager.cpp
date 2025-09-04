#include "materialManager.h"
#include "logger.h"
#include "macros.h"
#include "material.h"
#include "vulkanContext.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Static members:
	bool MaterialManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<Material>> MaterialManager::s_materials;



	// Initialization/Cleanup:
	void MaterialManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		uint32_t opaqueQueue = (uint32_t)Material::Queue::opaque;
		uint32_t skyboxQueue = (uint32_t)Material::Queue::skybox;
		uint32_t transparentQueue = (uint32_t)Material::Queue::transparent;
		Material::Type forwardOpaqueType = Material::Type::forwardOpaque;
		Material::Type forwardTransparentType = Material::Type::forwardTransparent;
		Material::Type shadowType = Material::Type::shadow;
		Material::Type skyboxType = Material::Type::skybox;
		Material::Type presentType = Material::Type::present;

		std::string directoryPath = (std::string)CORE_SHADERS_DIR + "/bin";
		//Material* testMaterial = new Material(Material::Type::forward, "testMaterial", directoryPath + "/test.vert.spv", directoryPath + "/test.frag.spv");
		//AddMaterial(testMaterial);

		Material* pErrorMaterial = new Material(forwardOpaqueType, "errorMaterial", opaqueQueue, directoryPath + "/error.vert.spv", directoryPath + "/error.frag.spv");
		AddMaterial(pErrorMaterial);

		Material* pDefaultMaterial = new Material(forwardOpaqueType, "defaultMaterial", opaqueQueue, directoryPath + "/default.vert.spv", directoryPath + "/default.frag.spv");
		AddMaterial(pDefaultMaterial);

		Material* pTransparentMaterial = new Material(forwardTransparentType, "transparentMaterial", transparentQueue, directoryPath + "/transparent.vert.spv", directoryPath + "/transparent.frag.spv");
		AddMaterial(pTransparentMaterial);

		Material* pPresentMaterial = new Material(presentType, "presentMaterial", opaqueQueue, directoryPath + "/present.vert.spv", directoryPath + "/present.frag.spv");
		AddMaterial(pPresentMaterial);

		Material* pVertexColorLitMaterial = new Material(forwardOpaqueType, "vertexColorLitMaterial", opaqueQueue, directoryPath + "/vertexColorLit.vert.spv", directoryPath + "/vertexColorLit.frag.spv");
		AddMaterial(pVertexColorLitMaterial);

		Material* pVertexColorUnlitMaterial = new Material(forwardOpaqueType, "vertexColorUnlitMaterial", opaqueQueue, directoryPath + "/vertexColorUnlit.vert.spv", directoryPath + "/vertexColorUnlit.frag.spv");
		AddMaterial(pVertexColorUnlitMaterial);

		Material* pNormalsMaterial = new Material(forwardOpaqueType, "normalMaterial", opaqueQueue, directoryPath + "/normals.vert.spv", directoryPath + "/normals.frag.spv");
		AddMaterial(pNormalsMaterial);

		Material* pShadowMaterial = new Material(shadowType, "shadowMaterial", opaqueQueue, directoryPath + "/shadow.vert.spv");
		AddMaterial(pShadowMaterial);

		Material* pSkyBoxMaterial = new Material(skyboxType, "skyboxMaterial", skyboxQueue, directoryPath + "/skybox.vert.spv", directoryPath + "/skybox.frag.spv");
		AddMaterial(pSkyBoxMaterial);

		Material* pSimpleLitMaterial = new Material(forwardOpaqueType, "simpleLitMaterial", opaqueQueue, directoryPath + "/simpleLit.vert.spv", directoryPath + "/simpleLit.frag.spv");
		AddMaterial(pSimpleLitMaterial);

		Material* pSimpleUnlitMaterial = new Material(forwardOpaqueType, "simpleUnlitMaterial", opaqueQueue, directoryPath + "/simpleUnlit.vert.spv", directoryPath + "/simpleUnlit.frag.spv");
		AddMaterial(pSimpleUnlitMaterial);

		// For testing spirv reflect:
		Material* pTest = new Material(forwardOpaqueType, "testMaterial", opaqueQueue, directoryPath + "/test.vert.spv", directoryPath + "/test.frag.spv");
		AddMaterial(pTest);

		// For testing the binding missmatch error:
		Material* pTestA = new Material(forwardOpaqueType, "testAMaterial", opaqueQueue, directoryPath + "/testA.vert.spv", directoryPath + "/testA.frag.spv");
		AddMaterial(pTestA);
		Material* pTestB = new Material(forwardOpaqueType, "testBMaterial", opaqueQueue, directoryPath + "/testB.vert.spv", directoryPath + "/testB.frag.spv");
		AddMaterial(pTestB);

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("MaterialManager initialized.");
		#endif
	}
	void MaterialManager::Clear()
	{
		Context::WaitDeviceIdle();
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
		Context::WaitDeviceIdle();
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