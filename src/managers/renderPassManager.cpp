#include "renderPassManager.h"
#include "vulkanMacros.h"
#include "vulkanRenderer.h"



// Static members:
bool RenderPassManager::isInitialized = false;
VulkanContext* RenderPassManager::context;
std::unordered_map<std::string, std::unique_ptr<RenderPass>> RenderPassManager::renderPasses;



// Initialization and cleanup:
void RenderPassManager::Init(VulkanContext* context)
{
	if (isInitialized)
		return;

	isInitialized = true;
	RenderPassManager::context = context;

	ForwardRenderPass* forwardRenderPass = new ForwardRenderPass(context);
	AddRenderPass("forwardRenderPass", forwardRenderPass);

	ShadowRenderPass* shadowRenderPass = new ShadowRenderPass(context);
	AddRenderPass("shadowRenderPass", shadowRenderPass);
}
void RenderPassManager::Clear()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	renderPasses.clear();
}
void RenderPassManager::RecreateRenderPasses()
{
	RenderPass* newForwardRenderPass = new ForwardRenderPass(context);
	DeleteRenderPass("forwardRenderPass");
	AddRenderPass("forwardRenderPass", newForwardRenderPass);
}



// Add/get/delete:
void RenderPassManager::AddRenderPass(const std::string name, RenderPass* renderPass)
{
	// If renderPass already contained in RenderPassManager, do nothing.
	if (renderPasses.emplace(name, std::unique_ptr<RenderPass>(renderPass)).second == false)
	{
		LOG_WARN("RenderPass with the name: {} already exists in RenderPassManager!", name);
		return;
	}
}
RenderPass* RenderPassManager::GetRenderPass(const std::string& name)
{
	auto it = renderPasses.find(name);
	if (it != renderPasses.end())
		return it->second.get();
	LOG_WARN("RenderPass '{}' not found!", name);
	return nullptr;
}
void RenderPassManager::DeleteRenderPass(const std::string& name)
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	renderPasses.erase(name);
}



// Debugging:
void RenderPassManager::PrintAllRenderPassNames()
{
	LOG_TRACE("Names of all managed renderPasses:");
	for (const auto& pair : renderPasses)
		LOG_TRACE(pair.first);
}