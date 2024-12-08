#include "SamplerManager.h"
#include "sampler.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



// Static members:
bool SamplerManager::isInitialized = false;
VulkanContext* SamplerManager::context;
std::unordered_map<std::string, std::unique_ptr<Sampler>> SamplerManager::samplers;



// Initialization and cleanup:
void SamplerManager::Init(VulkanContext* vulkanContext)
{
	if (isInitialized)
		return;

	isInitialized = true;
	context = vulkanContext;

	Sampler* colorSampler = Sampler::ColorSampler(context, "colorSampler");
	AddSampler(colorSampler->name, colorSampler);

	Sampler* shadowSampler = Sampler::ShadowSampler(context, "shadowSampler");
	AddSampler(shadowSampler->name, shadowSampler);

	Sampler* colorSampler2 = Sampler::ColorSampler(context, "colorSampler2");
	AddSampler(colorSampler2->name, colorSampler2);
}
void SamplerManager::Clear()
{
	VKA(vkDeviceWaitIdle(context->GetVkDevice()));
	samplers.clear();
}



// Add/get/delete:
void SamplerManager::AddSampler(const std::string name, Sampler* sampler)
{
	// If sampler already contained in SamplerManager, do nothing.
	if (samplers.emplace(name, std::unique_ptr<Sampler>(sampler)).second == false)
	{
		LOG_WARN("Sampler with the name: {} already exists in SamplerManager!", name);
		return;
	}
}
Sampler* SamplerManager::GetSampler(const std::string& name)
{
	auto it = samplers.find(name);
	if (it != samplers.end())
		return it->second.get();
	LOG_WARN("Sampler '{}' not found!", name);
	return nullptr;
}
void SamplerManager::DeleteSampler(const std::string& name)
{
	VKA(vkDeviceWaitIdle(context->GetVkDevice()));
	samplers.erase(name);
}



// Debugging:
void SamplerManager::PrintAllSamplerNames()
{
	LOG_TRACE("Names of all managed samplers:");
	for (const auto& pair : samplers)
		LOG_TRACE(pair.first);
}