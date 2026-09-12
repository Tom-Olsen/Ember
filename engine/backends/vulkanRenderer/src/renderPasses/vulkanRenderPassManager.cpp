#include "vulkanRenderPassManager.h"
#include "vulkanContext.h"
#include "vulkanDeferredGeometryRenderPass.h"
#include "vulkanDeferredLightingRenderPass.h"
#include "vulkanForwardOpaqueRenderPass.h"
#include "vulkanForwardTransparentRenderPass.h"
#include "vulkanGizmoRenderPass.h"
#include "vulkanOutlineRenderPass.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanRenderPass.h"
#include "vulkanRenderTargetResources.h"
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
	std::unique_ptr<ForwardOpaqueRenderPass> RenderPassManager::s_pForwardOpaqueRenderPass = nullptr;
	std::unique_ptr<ForwardTransparentRenderPass> RenderPassManager::s_pForwardTransparentRenderPass = nullptr;
	std::unique_ptr<PresentRenderPass> RenderPassManager::s_pPresentRenderPass = nullptr;

    

    // Public methods:
	// Initialization/Cleanup:
	void RenderPassManager::Init(const RenderTargetResources& renderTargets)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_pGizmoRenderPass = std::make_unique<GizmoRenderPass>(renderTargets);
		s_pOutlineRenderPass = std::make_unique<OutlineRenderPass>(renderTargets);
		s_pShadowRenderPass = std::make_unique<ShadowRenderPass>(renderTargets);
		s_pDeferredGeometryRenderPass = std::make_unique<DeferredGeometryRenderPass>(renderTargets);
		s_pDeferredLightingRenderPass = std::make_unique<DeferredLightingRenderPass>(renderTargets);
		s_pForwardOpaqueRenderPass = std::make_unique<ForwardOpaqueRenderPass>(renderTargets);
		s_pForwardTransparentRenderPass = std::make_unique<ForwardTransparentRenderPass>(renderTargets);
		s_pPresentRenderPass = std::make_unique<PresentRenderPass>();
	}
	void RenderPassManager::Clear()
	{
		s_pPresentRenderPass.reset();
		s_pForwardTransparentRenderPass.reset();
		s_pForwardOpaqueRenderPass.reset();
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
	ForwardOpaqueRenderPass* RenderPassManager::GetForwardOpaqueRenderPass()
	{
		return s_pForwardOpaqueRenderPass.get();
	}
	ForwardTransparentRenderPass* RenderPassManager::GetForwardTransparentRenderPass()
	{
		return s_pForwardTransparentRenderPass.get();
	}
	PresentRenderPass* RenderPassManager::GetPresentRenderPass()
	{
		return s_pPresentRenderPass.get();
	}
}