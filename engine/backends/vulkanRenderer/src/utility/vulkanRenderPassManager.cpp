#include "vulkanRenderPassManager.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanRenderPass.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanContext.h"



namespace vulkanRendererBackend
{
	// Static members:
	bool RenderPassManager::s_isInitialized = false;
	std::unique_ptr<ShadowRenderPass> RenderPassManager::s_pShadowRenderPass = nullptr;
	std::unique_ptr<ForwardRenderPass> RenderPassManager::s_pForwardRenderPass = nullptr;
	std::unique_ptr<PresentRenderPass> RenderPassManager::s_pPresentRenderPass = nullptr;



	// Initialization/Cleanup:
	void RenderPassManager::Init(uint32_t renderWidth, uint32_t renderHeight, uint32_t shadowMapResolution, uint32_t maxLightsCount)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_pShadowRenderPass = std::make_unique<ShadowRenderPass>(shadowMapResolution, maxLightsCount);
		s_pForwardRenderPass = std::make_unique<ForwardRenderPass>(renderWidth, renderHeight);
		s_pPresentRenderPass = std::make_unique<PresentRenderPass>();
	}
	void RenderPassManager::Clear()
	{
		Context::WaitDeviceIdle();
		s_pPresentRenderPass.reset();
		s_pForwardRenderPass.reset();
		s_pShadowRenderPass.reset();
	}
	void RenderPassManager::RecreateRenderPasses()
	{
		Context::WaitDeviceIdle();
		std::unique_ptr<PresentRenderPass> pNewPresentRenderPass = std::make_unique<PresentRenderPass>();
		std::swap(s_pPresentRenderPass, pNewPresentRenderPass);
	}



	// Getters:
	ShadowRenderPass* RenderPassManager::GetShadowRenderPass()
	{
		return s_pShadowRenderPass.get();
	}
	ForwardRenderPass* RenderPassManager::GetForwardRenderPass()
	{
		return s_pForwardRenderPass.get();
	}
	PresentRenderPass* RenderPassManager::GetPresentRenderPass()
	{
		return s_pPresentRenderPass.get();
	}
}