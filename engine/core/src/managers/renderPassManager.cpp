#include "renderPassManager.h"
#include "vulkanForwardRenderPass.h"
#include "logger.h"
#include "macros.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanRenderPass.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanContext.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Static members:
	bool RenderPassManager::s_isInitialized = false;
	uint32_t RenderPassManager::s_renderWidth = 0;
	uint32_t RenderPassManager::s_renderHeight = 0;
	std::unique_ptr<ShadowRenderPass> RenderPassManager::s_pShadowRenderPass = nullptr;
	std::unique_ptr<ForwardRenderPass> RenderPassManager::s_pForwardRenderPass = nullptr;
	std::unique_ptr<PresentRenderPass> RenderPassManager::s_pPresentRenderPass = nullptr;



	// Initialization/Cleanup:
	void RenderPassManager::Init(uint32_t renderWidth, uint32_t renderHeight)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_renderWidth = renderWidth;
		s_renderHeight = renderHeight;
		s_pShadowRenderPass = std::make_unique<ShadowRenderPass>();
		s_pForwardRenderPass = std::make_unique<ForwardRenderPass>(s_renderWidth, s_renderHeight);
		s_pPresentRenderPass = std::make_unique<PresentRenderPass>();

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("RenderPassManager initialized.");
		#endif
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