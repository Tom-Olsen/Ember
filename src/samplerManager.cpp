#include "SamplerManager.h"



// Static members:
bool SamplerManager::isInitialized = false;
VulkanContext* SamplerManager::context;
std::unordered_map<std::string, std::unique_ptr<VulkanSampler>> SamplerManager::samplers;



// Initialization and cleanup:
void SamplerManager::Init(VulkanContext* vulkanContext)
{
	if (isInitialized)
		return;

	isInitialized = true;
	context = vulkanContext;

	VulkanSampler* sampler = new VulkanSampler(context, "default");
	AddSampler(sampler->name, sampler);
}
void SamplerManager::Clear()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	samplers.clear();
}



// Add/get/delete:
void SamplerManager::AddSampler(const std::string name, VulkanSampler* sampler)
{
	// If sampler already contained in SamplerManager, do nothing.
	if (samplers.emplace(name, std::unique_ptr<VulkanSampler>(sampler)).second == false)
	{
		LOG_WARN("VulkanSampler with the name: {} already exists in SamplerManager!", name);
		return;
	}
}
VulkanSampler* SamplerManager::GetSampler(const std::string& name)
{
	auto it = samplers.find(name);
	if (it != samplers.end())
		return it->second.get();
	LOG_WARN("VulkanSampler '{}' not found!", name);
	return nullptr;
}
void SamplerManager::DeleteSampler(const std::string& name)
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	samplers.erase(name);
}



// Debugging:
void SamplerManager::PrintAllSamplerNames()
{
	LOG_TRACE("Names of all managed samplers:");
	for (const auto& pair : samplers)
		LOG_TRACE(pair.first);
}