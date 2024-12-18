#include "materialManager.h"
#include "material.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanRenderer.h"



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

	//Material* testMaterial = new Material(s_pContext, Material::Type::shading, "testMaterial", "../shaders/test.vert.spv", "../shaders/test.frag.spv");
	//AddMaterial(testMaterial);

	Material* pErrorMaterial = new Material(s_pContext, shadingType, "error", opaqueQueue, "../shaders/error.vert.spv", "../shaders/error.frag.spv");
	AddMaterial(pErrorMaterial);

	Material* pDefaultMaterial = new Material(s_pContext, shadingType, "default", opaqueQueue, "../shaders/default.vert.spv", "../shaders/default.frag.spv");
	AddMaterial(pDefaultMaterial);

	Material* pVertexColorLitMaterial = new Material(s_pContext, shadingType, "vertexColorLit", opaqueQueue, "../shaders/vertexColorLit.vert.spv", "../shaders/vertexColorLit.frag.spv");
	AddMaterial(pVertexColorLitMaterial);

	Material* pShadowMaterial = new Material(s_pContext, shadowType, "shadow", opaqueQueue, "../shaders/shadow.vert.spv");
	AddMaterial(pShadowMaterial);

	Material* pSkyBoxMaterial = new Material(s_pContext, skyboxType, "skybox", skyboxQueue, "../shaders/skybox.vert.spv", "../shaders/skybox.frag.spv");
	AddMaterial(pSkyBoxMaterial);

	Material* pSimpleLitMaterial = new Material(s_pContext, shadingType, "simpleLit", opaqueQueue, "../shaders/simpleLit.vert.spv", "../shaders/simpleLit.frag.spv");
	AddMaterial(pSimpleLitMaterial);

	// For testing the binding missmatch error:
	Material* pTestA = new Material(s_pContext, shadingType, "testA", opaqueQueue, "../shaders/testA.vert.spv", "../shaders/testA.frag.spv");
	AddMaterial(pTestA);
	Material* pTestB = new Material(s_pContext, shadingType, "testB", opaqueQueue, "../shaders/testB.vert.spv", "../shaders/testB.frag.spv");
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