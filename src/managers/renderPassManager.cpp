#include "renderPassManager.h"
#include "logger.h"
#include "renderPass.h"
#include "forwardRenderPass.h"
#include "shadowRenderPass.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Static members:
	bool RenderPassManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<RenderPass>> RenderPassManager::s_renderPasses;



	// Initialization and cleanup:
	void RenderPassManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		ForwardRenderPass* forwardRenderPass = new ForwardRenderPass();
		AddRenderPass("forwardRenderPass", forwardRenderPass);

		ShadowRenderPass* shadowRenderPass = new ShadowRenderPass();
		AddRenderPass("shadowRenderPass", shadowRenderPass);
	}
	void RenderPassManager::Clear()
	{
		VulkanContext::WaitDeviceIdle();
		s_renderPasses.clear();
	}
	void RenderPassManager::RecreateRenderPasses()
	{
		RenderPass* newForwardRenderPass = new ForwardRenderPass();
		DeleteRenderPass("forwardRenderPass");
		AddRenderPass("forwardRenderPass", newForwardRenderPass);
	}



	// Add/get/delete:
	void RenderPassManager::AddRenderPass(const std::string name, RenderPass* pRenderPass)
	{
		// If renderPass already contained in RenderPassManager, do nothing.
		if (s_renderPasses.emplace(name, std::unique_ptr<RenderPass>(pRenderPass)).second == false)
		{
			LOG_WARN("RenderPass with the name: {} already exists in RenderPassManager!", name);
			return;
		}
	}
	RenderPass* RenderPassManager::GetRenderPass(const std::string& name)
	{
		auto it = s_renderPasses.find(name);
		if (it != s_renderPasses.end())
			return it->second.get();
		LOG_WARN("RenderPass '{}' not found!", name);
		return nullptr;
	}
	void RenderPassManager::DeleteRenderPass(const std::string& name)
	{
		VulkanContext::WaitDeviceIdle();
		s_renderPasses.erase(name);
	}



	// Debugging:
	void RenderPassManager::PrintAllRenderPassNames()
	{
		LOG_TRACE("Names of all managed renderPasses:");
		for (const auto& pair : s_renderPasses)
			LOG_TRACE(pair.first);
	}
}