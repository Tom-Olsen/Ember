#include "vulkanRenderTargetResources.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanDeferredRenderingContract.h"
#include "vulkanDepthTexture2d.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanGBufferTexture2d.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSceneColorTexture2dPair.h"
#include "vulkanStorageTexture2d.h"
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	RenderTargetResources::RenderTargetResources(uint32_t renderWidth, uint32_t renderHeight, uint32_t shadowMapResolution, uint32_t maxLightsCount)
	{
		const uint32_t framesInFlight = Context::GetFramesInFlight();
		m_shadowMapResolution = shadowMapResolution;
		m_shadowMapLayerCount = maxLightsCount;
		m_pSceneColorTexturePair = std::make_unique<SceneColorTexture2dPair>(renderWidth, renderHeight, framesInFlight);
		m_pShadowMaps = std::make_unique<DepthTexture2dArray>(VK_FORMAT_D32_SFLOAT, shadowMapResolution, shadowMapResolution, maxLightsCount);

		m_pSceneDepthTextures.reserve(framesInFlight);
		m_pAlbedoTextures.reserve(framesInFlight);
		m_pNormalTextures.reserve(framesInFlight);
		m_pSurfacePropertiesTextures.reserve(framesInFlight);
		m_pOutlineTextures.reserve(framesInFlight);
		m_pGizmoTextures.reserve(framesInFlight);
		m_pExpandedOutlineMaskTextures.reserve(framesInFlight);
		m_pHorizontalExpandedOutlineMaskTextures.reserve(framesInFlight);

		for (uint32_t frameIndex = 0; frameIndex < framesInFlight; frameIndex++)
		{
			m_pSceneDepthTextures.push_back(std::make_unique<DepthTexture2d>(deferredRenderingContract::depthFormat, renderWidth, renderHeight));
			m_pAlbedoTextures.push_back(std::make_unique<GBufferTexture2d>(deferredRenderingContract::albedoFormat, renderWidth, renderHeight));
			m_pNormalTextures.push_back(std::make_unique<GBufferTexture2d>(deferredRenderingContract::normalFormat, renderWidth, renderHeight));
			m_pSurfacePropertiesTextures.push_back(std::make_unique<GBufferTexture2d>(deferredRenderingContract::surfacePropertiesFormat, renderWidth, renderHeight));
			m_pOutlineTextures.push_back(std::make_unique<RenderTexture2d>(VK_FORMAT_R8_UNORM, renderWidth, renderHeight));
			m_pGizmoTextures.push_back(std::make_unique<RenderTexture2d>(VK_FORMAT_R16G16B16A16_SFLOAT, renderWidth, renderHeight));
			m_pExpandedOutlineMaskTextures.push_back(std::make_unique<StorageTexture2d>(VK_FORMAT_R8_UNORM, renderWidth, renderHeight));
			m_pHorizontalExpandedOutlineMaskTextures.push_back(std::make_unique<StorageTexture2d>(VK_FORMAT_R8_UNORM, renderWidth, renderHeight));

			m_pSceneDepthTextures[frameIndex]->SetDebugName("SceneDepthTexture_Frame" + std::to_string(frameIndex));
			m_pAlbedoTextures[frameIndex]->SetDebugName("GBufferAlbedoTexture_Frame" + std::to_string(frameIndex));
			m_pNormalTextures[frameIndex]->SetDebugName("GBufferNormalTexture_Frame" + std::to_string(frameIndex));
			m_pSurfacePropertiesTextures[frameIndex]->SetDebugName("GBufferSurfacePropertiesTexture_Frame" + std::to_string(frameIndex));
			m_pOutlineTextures[frameIndex]->SetDebugName("RenderTexture_Outline_Frame" + std::to_string(frameIndex));
			m_pGizmoTextures[frameIndex]->SetDebugName("RenderTexture_Gizmo_Frame" + std::to_string(frameIndex));
			m_pExpandedOutlineMaskTextures[frameIndex]->SetDebugName("StorageTexture_ExpandedOutlineMask_Frame" + std::to_string(frameIndex));
			m_pHorizontalExpandedOutlineMaskTextures[frameIndex]->SetDebugName("StorageTexture_HorizontalExpandedOutlineMask_Frame" + std::to_string(frameIndex));

			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::BottomOfPipe::none;
			m_pSceneDepthTextures[frameIndex]->GetVmaImage()->TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, srcStage, dstStage, srcAccessMask, dstAccessMask);
			m_pAlbedoTextures[frameIndex]->GetVmaImage()->TransitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, srcStage, dstStage, srcAccessMask, dstAccessMask);
			m_pNormalTextures[frameIndex]->GetVmaImage()->TransitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, srcStage, dstStage, srcAccessMask, dstAccessMask);
			m_pSurfacePropertiesTextures[frameIndex]->GetVmaImage()->TransitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, srcStage, dstStage, srcAccessMask, dstAccessMask);
			m_pOutlineTextures[frameIndex]->GetVmaImage()->TransitionLayout(VK_IMAGE_LAYOUT_GENERAL, srcStage, dstStage, srcAccessMask, dstAccessMask);
			m_pGizmoTextures[frameIndex]->GetVmaImage()->TransitionLayout(VK_IMAGE_LAYOUT_GENERAL, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}

		CreateGizmoMsaaImages();
		CreateGizmoDepthImages();
	}
	RenderTargetResources::~RenderTargetResources() = default;



	// Movable:
	RenderTargetResources::RenderTargetResources(RenderTargetResources&& other) noexcept = default;
	RenderTargetResources& RenderTargetResources::operator=(RenderTargetResources&& other) noexcept = default;



	// Getters:
	uint32_t RenderTargetResources::GetFrameCount() const
	{
		return static_cast<uint32_t>(m_pSceneDepthTextures.size());
	}
	uint32_t RenderTargetResources::GetShadowMapResolution() const
	{
		return m_shadowMapResolution;
	}
	uint32_t RenderTargetResources::GetShadowMapLayerCount() const
	{
		return m_shadowMapLayerCount;
	}
	SceneColorTexture2dPair& RenderTargetResources::GetSceneColorTexturePair()
	{
		return *m_pSceneColorTexturePair;
	}
	const SceneColorTexture2dPair& RenderTargetResources::GetSceneColorTexturePair() const
	{
		return *m_pSceneColorTexturePair;
	}
	DepthTexture2d& RenderTargetResources::GetSceneDepthTexture(uint32_t frameIndex)
	{
		return *m_pSceneDepthTextures.at(frameIndex);
	}
	const DepthTexture2d& RenderTargetResources::GetSceneDepthTexture(uint32_t frameIndex) const
	{
		return *m_pSceneDepthTextures.at(frameIndex);
	}
	GBufferTexture2d& RenderTargetResources::GetAlbedoTexture(uint32_t frameIndex)
	{
		return *m_pAlbedoTextures.at(frameIndex);
	}
	const GBufferTexture2d& RenderTargetResources::GetAlbedoTexture(uint32_t frameIndex) const
	{
		return *m_pAlbedoTextures.at(frameIndex);
	}
	GBufferTexture2d& RenderTargetResources::GetNormalTexture(uint32_t frameIndex)
	{
		return *m_pNormalTextures.at(frameIndex);
	}
	const GBufferTexture2d& RenderTargetResources::GetNormalTexture(uint32_t frameIndex) const
	{
		return *m_pNormalTextures.at(frameIndex);
	}
	GBufferTexture2d& RenderTargetResources::GetSurfacePropertiesTexture(uint32_t frameIndex)
	{
		return *m_pSurfacePropertiesTextures.at(frameIndex);
	}
	const GBufferTexture2d& RenderTargetResources::GetSurfacePropertiesTexture(uint32_t frameIndex) const
	{
		return *m_pSurfacePropertiesTextures.at(frameIndex);
	}
	RenderTexture2d& RenderTargetResources::GetOutlineTexture(uint32_t frameIndex)
	{
		return *m_pOutlineTextures.at(frameIndex);
	}
	const RenderTexture2d& RenderTargetResources::GetOutlineTexture(uint32_t frameIndex) const
	{
		return *m_pOutlineTextures.at(frameIndex);
	}
	RenderTexture2d& RenderTargetResources::GetGizmoTexture(uint32_t frameIndex)
	{
		return *m_pGizmoTextures.at(frameIndex);
	}
	const RenderTexture2d& RenderTargetResources::GetGizmoTexture(uint32_t frameIndex) const
	{
		return *m_pGizmoTextures.at(frameIndex);
	}
	VmaImage& RenderTargetResources::GetGizmoMsaaImage(uint32_t frameIndex)
	{
		return *m_pGizmoMsaaImages.at(frameIndex);
	}
	const VmaImage& RenderTargetResources::GetGizmoMsaaImage(uint32_t frameIndex) const
	{
		return *m_pGizmoMsaaImages.at(frameIndex);
	}
	VmaImage& RenderTargetResources::GetGizmoDepthImage(uint32_t frameIndex)
	{
		return *m_pGizmoDepthImages.at(frameIndex);
	}
	const VmaImage& RenderTargetResources::GetGizmoDepthImage(uint32_t frameIndex) const
	{
		return *m_pGizmoDepthImages.at(frameIndex);
	}
	StorageTexture2d& RenderTargetResources::GetExpandedOutlineMaskTexture(uint32_t frameIndex)
	{
		return *m_pExpandedOutlineMaskTextures.at(frameIndex);
	}
	const StorageTexture2d& RenderTargetResources::GetExpandedOutlineMaskTexture(uint32_t frameIndex) const
	{
		return *m_pExpandedOutlineMaskTextures.at(frameIndex);
	}
	StorageTexture2d& RenderTargetResources::GetHorizontalExpandedOutlineMaskTexture(uint32_t frameIndex)
	{
		return *m_pHorizontalExpandedOutlineMaskTextures.at(frameIndex);
	}
	const StorageTexture2d& RenderTargetResources::GetHorizontalExpandedOutlineMaskTexture(uint32_t frameIndex) const
	{
		return *m_pHorizontalExpandedOutlineMaskTextures.at(frameIndex);
	}
	DepthTexture2dArray& RenderTargetResources::GetShadowMaps()
	{
		return *m_pShadowMaps;
	}
	const DepthTexture2dArray& RenderTargetResources::GetShadowMaps() const
	{
		return *m_pShadowMaps;
	}



	// Private methods:
	void RenderTargetResources::CreateGizmoMsaaImages()
	{
		if (Context::GetMsaaSamples() == VK_SAMPLE_COUNT_1_BIT)
			return;

		m_pGizmoMsaaImages.reserve(GetFrameCount());
		for (uint32_t frameIndex = 0; frameIndex < GetFrameCount(); frameIndex++)
		{
			VkImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;

			const RenderTexture2d& gizmoTexture = GetGizmoTexture(frameIndex);
			VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent = { gizmoTexture.GetWidth(), gizmoTexture.GetHeight(), 1 };
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = gizmoTexture.GetFormat();
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = Context::GetMsaaSamples();
			imageInfo.flags = 0;

			VmaAllocationCreateInfo allocationInfo = {};
			allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

			DeviceQueue queue = Context::GetLogicalDevice()->GetGraphicsQueue();
			VkDeviceSize allocationSize = 8ull * imageInfo.extent.width * imageInfo.extent.height * imageInfo.samples;
			m_pGizmoMsaaImages.push_back(std::make_unique<VmaImage>(imageInfo, allocationInfo, allocationSize, subresourceRange, VK_IMAGE_VIEW_TYPE_2D, queue));
			NAME_VK_OBJECT(m_pGizmoMsaaImages[frameIndex]->GetVkImage(), "Image_GizmoMsaa_Frame" + std::to_string(frameIndex));
			NAME_VK_OBJECT(m_pGizmoMsaaImages[frameIndex]->GetVkImageView(), "ImageView_GizmoMsaa_Frame" + std::to_string(frameIndex));
		}
	}
	void RenderTargetResources::CreateGizmoDepthImages()
	{
		m_pGizmoDepthImages.reserve(GetFrameCount());
		for (uint32_t frameIndex = 0; frameIndex < GetFrameCount(); frameIndex++)
		{
			VkImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;

			const RenderTexture2d& gizmoTexture = GetGizmoTexture(frameIndex);
			VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent = { gizmoTexture.GetWidth(), gizmoTexture.GetHeight(), 1 };
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = VK_FORMAT_D32_SFLOAT;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = Context::GetMsaaSamples();
			imageInfo.flags = 0;

			VmaAllocationCreateInfo allocationInfo = {};
			allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

			DeviceQueue queue = Context::GetLogicalDevice()->GetGraphicsQueue();
			VkDeviceSize allocationSize = 4ull * imageInfo.extent.width * imageInfo.extent.height * imageInfo.samples;
			m_pGizmoDepthImages.push_back(std::make_unique<VmaImage>(imageInfo, allocationInfo, allocationSize, subresourceRange, VK_IMAGE_VIEW_TYPE_2D, queue));
			NAME_VK_OBJECT(m_pGizmoDepthImages[frameIndex]->GetVkImage(), "Image_GizmoDepth_Frame" + std::to_string(frameIndex));
			NAME_VK_OBJECT(m_pGizmoDepthImages[frameIndex]->GetVkImageView(), "ImageView_GizmoDepth_Frame" + std::to_string(frameIndex));

			m_pGizmoDepthImages[frameIndex]->TransitionLayout(
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
				AccessMasks::TopOfPipe::none,
				AccessMasks::EarlyFragmentTest::depthStencilAttachmentRead);
		}
	}
}