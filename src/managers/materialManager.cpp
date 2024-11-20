#include "materialManager.h"
#include "vulkanMacros.h"



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

	Material* defaultMaterial = new Material(context, Material::Type::forward, "defaultMaterial", "../shaders/default.vert.spv", "../shaders/default.frag.spv");
	AddMaterial(defaultMaterial);

	Material* testMaterial = new Material(context, Material::Type::forward, "testMaterial", "../shaders/test.vert.spv", "../shaders/test.frag.spv");
	AddMaterial(testMaterial);

	Material* colorMaterial = new Material(context, Material::Type::forward, "colorMaterial", "../shaders/color.vert.spv", "../shaders/color.frag.spv");
	AddMaterial(colorMaterial);

	Material* shadowMaterial = new Material(context, Material::Type::shadow, "shadowMaterial", "../shaders/shadow.vert.spv");
	AddMaterial(shadowMaterial);
}
void MaterialManager::Clear()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	materials.clear();
}



// Add/get/delete:
void MaterialManager::AddMaterial(Material* material)
{
	// If material already contained in MaterialManager, do nothing.
	if (materials.emplace(material->name, std::unique_ptr<Material>(material)).second == false)
	{
		LOG_WARN("Material with the name: {} already exists in MaterialManager!", material->name);
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