#include "materialManager.h"
#include "logger.h"
#include "material.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Static members:
	bool MaterialManager::s_isInitialized = false;
	VulkanContext* MaterialManager::s_pContext;
	std::unordered_map<std::string, std::unique_ptr<Material>> MaterialManager::s_materials;



	// Initialization and cleanup:
	void MaterialManager::Init(VulkanContext* pContext)
	{
		if (s_isInitialized)
			return;

		s_isInitialized = true;
		s_pContext = pContext;
		Material::RenderQueue opaqueQueue = Material::RenderQueue::opaque;
		Material::RenderQueue transparentQueue = Material::RenderQueue::transparent;
		Material::RenderQueue skyboxQueue = Material::RenderQueue::skybox;
		Material::Type shadingType = Material::Type::shading;
		Material::Type shadowType = Material::Type::shadow;
		Material::Type skyboxType = Material::Type::skybox;

		//Material* testMaterial = new Material(s_pContext, Material::Type::shading, "testMaterial", "../src/shaders/test.vert.spv", "../src/shaders/test.frag.spv");
		//AddMaterial(testMaterial);

		Material* pErrorMaterial = new Material(s_pContext, shadingType, "error", opaqueQueue, "../src/shaders/error.vert.spv", "../src/shaders/error.frag.spv");
		AddMaterial(pErrorMaterial);

		Material* pDefaultMaterial = new Material(s_pContext, shadingType, "default", opaqueQueue, "../src/shaders/default.vert.spv", "../src/shaders/default.frag.spv");
		AddMaterial(pDefaultMaterial);

		Material* pVertexColorLitMaterial = new Material(s_pContext, shadingType, "vertexColorLit", opaqueQueue, "../src/shaders/vertexColorLit.vert.spv", "../src/shaders/vertexColorLit.frag.spv");
		AddMaterial(pVertexColorLitMaterial);

		Material* pVertexColorUnlitMaterial = new Material(s_pContext, shadingType, "vertexColorUnlit", opaqueQueue, "../src/shaders/vertexColorUnlit.vert.spv", "../src/shaders/vertexColorUnlit.frag.spv");
		AddMaterial(pVertexColorUnlitMaterial);

		Material* pNormalsMaterial = new Material(s_pContext, shadingType, "normals", opaqueQueue, "../src/shaders/normals.vert.spv", "../src/shaders/normals.frag.spv");
		AddMaterial(pNormalsMaterial);

		Material* pShadowMaterial = new Material(s_pContext, shadowType, "shadow", opaqueQueue, "../src/shaders/shadow.vert.spv");
		AddMaterial(pShadowMaterial);

		Material* pSkyBoxMaterial = new Material(s_pContext, skyboxType, "skybox", skyboxQueue, "../src/shaders/skybox.vert.spv", "../src/shaders/skybox.frag.spv");
		AddMaterial(pSkyBoxMaterial);

		Material* pSimpleLitMaterial = new Material(s_pContext, shadingType, "simpleLit", opaqueQueue, "../src/shaders/simpleLit.vert.spv", "../src/shaders/simpleLit.frag.spv");
		AddMaterial(pSimpleLitMaterial);

		Material* pSimpleUnlitMaterial = new Material(s_pContext, shadingType, "simpleUnlit", opaqueQueue, "../src/shaders/simpleUnlit.vert.spv", "../src/shaders/simpleUnlit.frag.spv");
		AddMaterial(pSimpleUnlitMaterial);

		// For testing the binding missmatch error:
		Material* pTestA = new Material(s_pContext, shadingType, "testA", opaqueQueue, "../src/shaders/testA.vert.spv", "../src/shaders/testA.frag.spv");
		AddMaterial(pTestA);
		Material* pTestB = new Material(s_pContext, shadingType, "testB", opaqueQueue, "../src/shaders/testB.vert.spv", "../src/shaders/testB.frag.spv");
		AddMaterial(pTestB);
	}
	void MaterialManager::Clear()
	{
		s_pContext->WaitDeviceIdle();
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
		s_pContext->WaitDeviceIdle();
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