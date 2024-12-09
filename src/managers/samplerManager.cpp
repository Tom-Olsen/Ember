#include "SamplerManager.h"
#include "sampler.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



// Static members:
bool SamplerManager::s_isInitialized = false;
VulkanContext* SamplerManager::s_pContext;
std::unordered_map<std::string, std::unique_ptr<Sampler>> SamplerManager::s_samplers;



// Initialization and cleanup:
void SamplerManager::Init(VulkanContext* pContext)
{
	if (s_isInitialized)
		return;

	s_isInitialized = true;
	s_pContext = pContext;

	Sampler* colorSampler = Sampler::ColorSampler(s_pContext, "colorSampler");
	AddSampler(colorSampler);

	Sampler* shadowSampler = Sampler::ShadowSampler(s_pContext, "shadowSampler");
	AddSampler(shadowSampler);

	Sampler* colorSampler2 = Sampler::ColorSampler(s_pContext, "colorSampler2");
	AddSampler(colorSampler2);
}
void SamplerManager::Clear()
{
	s_pContext->WaitDeviceIdle();
	s_samplers.clear();
}



// Add/get/delete:
void SamplerManager::AddSampler(Sampler* pSampler)
{
	// If sampler already contained in SamplerManager, do nothing.
	if (s_samplers.emplace(pSampler->GetName(), std::unique_ptr<Sampler>(pSampler)).second == false)
	{
		LOG_WARN("Sampler with the name: {} already exists in SamplerManager!", pSampler->GetName());
		return;
	}
}
Sampler* SamplerManager::GetSampler(const std::string& name)
{
	auto it = s_samplers.find(name);
	if (it != s_samplers.end())
		return it->second.get();
	LOG_WARN("Sampler '{}' not found!", name);
	return nullptr;
}
void SamplerManager::DeleteSampler(const std::string& name)
{
	s_pContext->WaitDeviceIdle();
	s_samplers.erase(name);
}



// Debugging:
void SamplerManager::PrintAllSamplerNames()
{
	LOG_TRACE("Names of all managed s_samplers:");
	for (const auto& pair : s_samplers)
		LOG_TRACE(pair.first);
}