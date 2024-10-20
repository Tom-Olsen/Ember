#include "materialManager.h"



// Static members:
VulkanContext* MaterialManager::context;
std::unordered_map<std::string, std::unique_ptr<Material>> MaterialManager::materials;



// Initialization and cleanup:
void MaterialManager::InitDefaultMaterials(VulkanContext* vulkanContext)
{
	context = vulkanContext;
	Material* defaultMaterial = new Material(context, std::string("../shaders/vert.spv"), std::string("../shaders/frag.spv"), "defaultMaterial");
	AddMaterial(defaultMaterial->name, defaultMaterial);
}
void MaterialManager::ClearMaterialMap()
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