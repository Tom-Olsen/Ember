#include "vulkanRenderPassManager.h"
#include "vulkanContext.h"
#include "vulkanDeferredGeometryRenderPass.h"
#include "vulkanDeferredLightingRenderPass.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanGizmoRenderPass.h"
#include "vulkanOutlineRenderPass.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanRenderPass.h"
#include "vulkanShadowRenderPass.h"



namespace vulkanRendererBackend
{
	// Static members:
	bool RenderPassManager::s_isInitialized = false;
	std::unique_ptr<GizmoRenderPass> RenderPassManager::s_pGizmoRenderPass = nullptr;
	std::unique_ptr<OutlineRenderPass> RenderPassManager::s_pOutlineRenderPass = nullptr;
	std::unique_ptr<ShadowRenderPass> RenderPassManager::s_pShadowRenderPass = nullptr;
	std::unique_ptr<DeferredGeometryRenderPass> RenderPassManager::s_pDeferredGeometryRenderPass = nullptr;
	std::unique_ptr<DeferredLightingRenderPass> RenderPassManager::s_pDeferredLightingRenderPass = nullptr;
	std::unique_ptr<ForwardRenderPass> RenderPassManager::s_pForwardRenderPass = nullptr;
	std::unique_ptr<PresentRenderPass> RenderPassManager::s_pPresentRenderPass = nullptr;

    

    // Public methods:
	// Initialization/Cleanup:
	void RenderPassManager::Init(
		uint32_t renderWidth,
		uint32_t renderHeight,
		uint32_t shadowMapResolution,
		uint32_t maxLightsCount,
		const std::vector<std::unique_ptr<RenderTexture2d>>& pSceneColorTextures,
		const std::vector<std::unique_ptr<RenderTexture2d>>& pSecondarySceneColorTextures,
		const std::vector<std::unique_ptr<DepthTexture2d>>& pSceneDepthTextures)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_pGizmoRenderPass = std::make_unique<GizmoRenderPass>(renderWidth, renderHeight);
		s_pOutlineRenderPass = std::make_unique<OutlineRenderPass>(renderWidth, renderHeight);
		s_pShadowRenderPass = std::make_unique<ShadowRenderPass>(shadowMapResolution, maxLightsCount);
		s_pDeferredGeometryRenderPass = std::make_unique<DeferredGeometryRenderPass>(renderWidth, renderHeight, pSceneDepthTextures);
		s_pDeferredLightingRenderPass = std::make_unique<DeferredLightingRenderPass>(pSceneColorTextures, pSceneDepthTextures, *s_pDeferredGeometryRenderPass);
		s_pForwardRenderPass = std::make_unique<ForwardRenderPass>(pSceneColorTextures, pSecondarySceneColorTextures, pSceneDepthTextures);
		s_pPresentRenderPass = std::make_unique<PresentRenderPass>();
	}
	void RenderPassManager::Clear()
	{
		s_pPresentRenderPass.reset();
		s_pForwardRenderPass.reset();
		s_pDeferredLightingRenderPass.reset();
		s_pDeferredGeometryRenderPass.reset();
		s_pShadowRenderPass.reset();
		s_pOutlineRenderPass.reset();
		s_pGizmoRenderPass.reset();
		s_isInitialized = false;
	}
	void RenderPassManager::RecreateRenderPasses()
	{
		Context::WaitDeviceIdle();
		std::unique_ptr<PresentRenderPass> pNewPresentRenderPass = std::make_unique<PresentRenderPass>();
		std::swap(s_pPresentRenderPass, pNewPresentRenderPass);
	}



	// Getters:
	GizmoRenderPass* RenderPassManager::GetGizmoRenderPass()
	{
		return s_pGizmoRenderPass.get();
	}
	OutlineRenderPass* RenderPassManager::GetOutlineRenderPass()
	{
		return s_pOutlineRenderPass.get();
	}
	ShadowRenderPass* RenderPassManager::GetShadowRenderPass()
	{
		return s_pShadowRenderPass.get();
	}
	DeferredGeometryRenderPass* RenderPassManager::GetDeferredGeometryRenderPass()
	{
		return s_pDeferredGeometryRenderPass.get();
	}
	DeferredLightingRenderPass* RenderPassManager::GetDeferredLightingRenderPass()
	{
		return s_pDeferredLightingRenderPass.get();
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