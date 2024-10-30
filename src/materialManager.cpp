#include "materialManager.h"



// Static members:
bool MaterialManager::isInitialized = false;
VulkanContext* MaterialManager::context;
std::unordered_map<std::string, std::unique_ptr<Material>> MaterialManager::materials;



// Initialization and cleanup:
void MaterialManager::Init(VulkanContext* context, VulkanRenderer* renderer)
{
	if (isInitialized)
		return;

	isInitialized = true;
	MaterialManager::context = context;

	Material* defaultMaterial = new Material(context, &renderer->forwardRenderPass->renderPass, "../shaders/vertDefault.spv", "../shaders/fragDefault.spv", "defaultMaterial");
	AddMaterial(defaultMaterial->name, defaultMaterial);

	Material* colorMaterial = new Material(context, &renderer->forwardRenderPass->renderPass, "../shaders/vertColor.spv", "../shaders/fragColor.spv", "colorMaterial");
	AddMaterial(colorMaterial->name, colorMaterial);

	Material* shadowMaterial = new Material(context, &renderer->shadowRenderPass->renderPass, "../shaders/vertShadow.spv", "shadowMaterial");
	AddMaterial(shadowMaterial->name, shadowMaterial);

	Material* shadowMapMaterial = new Material(context, &renderer->forwardRenderPass->renderPass, "../shaders/vertShadowMap.spv", "../shaders/fragShadowMap.spv", "shadowMapMaterial");
	AddMaterial(shadowMapMaterial->name, shadowMapMaterial);
}
void MaterialManager::Clear()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	materials.clear();
}



// Add/get/delete:
void MaterialManager::AddMaterial(const std::string name, Material* material)
{
	// If material already contained in MaterialManager, do nothing.
	if (materials.emplace(name, std::unique_ptr<Material>(material)).second == false)
	{
		LOG_WARN("Material with the name: {} already exists in MaterialManager!", name);
		return;
	}
}
Material* MaterialManager::GetMaterial(const std::string& name)
{
	auto it = materials.find(name);
	if (it != materials.end())
		return it->second.get();
	LOG_WARN("Material '{}' not found!", name);
	return nullptr;
}
void MaterialManager::DeleteMaterial(const std::string& name)
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	materials.erase(name);
}



// Debugging:
void MaterialManager::PrintAllMaterialNames()
{
	LOG_TRACE("Names of all managed materials:");
	for (const auto& pair : materials)
		LOG_TRACE(pair.first);
}