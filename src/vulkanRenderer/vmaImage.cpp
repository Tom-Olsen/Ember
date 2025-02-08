#include "vmaImage.h"
#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Constructors/Destructor:
	VmaImage::VmaImage(VulkanContext* pContext, VkImageCreateInfo* pImageInfo, VmaAllocationCreateInfo* pAllocationInfo, VkImageSubresourceRange* pSubresourceRange)
	{
		m_pContext = pContext;
		m_pImageInfo = std::unique_ptr<VkImageCreateInfo>(pImageInfo);
		m_pAllocationInfo = std::unique_ptr<VmaAllocationCreateInfo>(pAllocationInfo);
		m_pSubresourceRange = std::unique_ptr<VkImageSubresourceRange>(pSubresourceRange);
		m_layout = pImageInfo->initialLayout;

		// Create image:
		VKA(vmaCreateImage(m_pContext->GetVmaAllocator(), m_pImageInfo.get(), m_pAllocationInfo.get(), &m_image, &m_allocation, nullptr));

		// Determine view type:
		VkImageViewType viewType = VkImageViewType((int)m_pImageInfo->imageType);
		if (m_pImageInfo->flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
			viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		else if (m_pImageInfo->arrayLayers > 1)
		{
			if (m_pImageInfo->imageType == VK_IMAGE_TYPE_1D)
				viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
			else if (m_pImageInfo->imageType == VK_IMAGE_TYPE_2D)
				viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		}

		// Create image view:
		VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewInfo.image = m_image;
		viewInfo.viewType = viewType;
		viewInfo.format = m_pImageInfo->format;
		viewInfo.subresourceRange = *m_pSubresourceRange;
		VKA(vkCreateImageView(m_pContext->GetVkDevice(), &viewInfo, nullptr, &m_imageView));
	}
	VmaImage::~VmaImage()
	{
		vmaDestroyImage(m_pContext->GetVmaAllocator(), m_image, m_allocation);
		vkDestroyImageView(m_pContext->GetVkDevice(), m_imageView, nullptr);
	}



	// Public methods:
	// Getters:
	const VkImage& VmaImage::GetVkImage() const
	{
		return m_image;
	}
	const VmaAllocation& VmaImage::GetVmaAllocation() const
	{
		return m_allocation;
	}
	const VkImageView& VmaImage::GetVkImageView() const
	{
		return m_imageView;
	}
	const VkImageCreateInfo* const VmaImage::GetVkImageCreateInfo() const
	{
		return m_pImageInfo.get();
	}
	const VmaAllocationCreateInfo* const VmaImage::GetVmaAllocationCreateInfo() const
	{
		return m_pAllocationInfo.get();
	}
	const VkImageSubresourceRange* const VmaImage::GetSubresourceRange() const
	{
		return m_pSubresourceRange.get();
	}
	const VkImageLayout& VmaImage::GetLayout() const
	{
		return m_layout;
	}
	uint64_t VmaImage::GetWidth() const
	{
		return m_pImageInfo->extent.width;
	}
	uint64_t VmaImage::GetHeight() const
	{
		return m_pImageInfo->extent.height;
	}
	uint64_t VmaImage::GetDepth() const
	{
		return m_pImageInfo->extent.depth;
	}
	const VkExtent3D& VmaImage::GetExtent() const
	{
		return m_pImageInfo->extent;
	}
	VkImageSubresourceLayers VmaImage::GetSubresourceLayers() const
	{
		VkImageSubresourceLayers subresourceLayers = {};
		subresourceLayers.aspectMask = m_pSubresourceRange->aspectMask;
		subresourceLayers.mipLevel = m_pSubresourceRange->baseMipLevel;
		subresourceLayers.baseArrayLayer = m_pSubresourceRange->baseArrayLayer;
		subresourceLayers.layerCount = m_pSubresourceRange->layerCount;
		return subresourceLayers;
	}



	// Transitions etc.:
	void VmaImage::TransitionLayoutUndefinedToTransfer()
	{
		// Transition is executed on transferQueue.
		VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(m_pContext, m_pContext->pLogicalDevice->GetTransferQueue());

		VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.srcAccessMask = VK_ACCESS_NONE;					// types of memory access allowed before the barrier
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;	// types of memory access allowed after the barrier
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_image;
		barrier.subresourceRange = *m_pSubresourceRange;

		VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;	// Immediatly
		VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;		// Before transfer stage
		vkCmdPipelineBarrier(
			command.GetVkCommandBuffer(),
			srcStage, dstStage,
			0,	// dependency flags, typically 0
			0, nullptr,				// memory barriers
			0, nullptr,	// buffer memory barriers
			1, &barrier);	// image memory barriers

		VulkanCommand::EndSingleTimeCommand(m_pContext, command, m_pContext->pLogicalDevice->GetTransferQueue());

		m_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	}
	void VmaImage::HandoffTransferToGraphicsQueue()
	{
		// On transition ownership of the image is transferred from the transferQueue to the graphicsQueue.
		{
			VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(m_pContext, m_pContext->pLogicalDevice->GetTransferQueue());

			VkImageMemoryBarrier releaseBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
			releaseBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			releaseBarrier.dstAccessMask = VK_ACCESS_NONE;
			releaseBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			releaseBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			releaseBarrier.srcQueueFamilyIndex = m_pContext->pLogicalDevice->GetTransferQueue().familyIndex;
			releaseBarrier.dstQueueFamilyIndex = m_pContext->pLogicalDevice->GetGraphicsQueue().familyIndex;
			releaseBarrier.image = m_image;
			releaseBarrier.subresourceRange = *m_pSubresourceRange;

			VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;			// after transfer stage
			VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;	// wait for all transfers to finish
			vkCmdPipelineBarrier(
				command.GetVkCommandBuffer(),
				srcStage, dstStage,
				0,	// dependency flags, typically 0
				0, nullptr,						// memory barriers
				0, nullptr,			// buffer memory barriers
				1, &releaseBarrier);	// image memory barriers

			VulkanCommand::EndSingleTimeCommand(m_pContext, command, m_pContext->pLogicalDevice->GetTransferQueue());

			m_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}
		{
			VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(m_pContext, m_pContext->pLogicalDevice->GetGraphicsQueue());

			VkImageMemoryBarrier acquireBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
			acquireBarrier.srcAccessMask = VK_ACCESS_NONE;
			acquireBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;	// rdy for mipmapping
			acquireBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			acquireBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			acquireBarrier.srcQueueFamilyIndex = m_pContext->pLogicalDevice->GetTransferQueue().familyIndex;
			acquireBarrier.dstQueueFamilyIndex = m_pContext->pLogicalDevice->GetGraphicsQueue().familyIndex;
			acquireBarrier.image = m_image;
			acquireBarrier.subresourceRange = *m_pSubresourceRange;

			VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;	// as early as possible
			VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;		// rdy for more transfer commands
			vkCmdPipelineBarrier(
				command.GetVkCommandBuffer(),
				srcStage, dstStage,
				0,	// dependency flags, typically 0
				0, nullptr,						// memory barriers
				0, nullptr,			// buffer memory barriers
				1, &acquireBarrier);	// image memory barriers

			VulkanCommand::EndSingleTimeCommand(m_pContext, command, m_pContext->pLogicalDevice->GetGraphicsQueue());

			m_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}
	}
	void VmaImage::TransitionLayoutTransferToShaderRead()
	{
		// On transition ownership of the image is transferred from the transferQueue to the graphicsQueue.
		{
			VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(m_pContext, m_pContext->pLogicalDevice->GetTransferQueue());

			VkImageMemoryBarrier releaseBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
			releaseBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			releaseBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			releaseBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			releaseBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			releaseBarrier.srcQueueFamilyIndex = m_pContext->pLogicalDevice->GetTransferQueue().familyIndex;
			releaseBarrier.dstQueueFamilyIndex = m_pContext->pLogicalDevice->GetGraphicsQueue().familyIndex;
			releaseBarrier.image = m_image;
			releaseBarrier.subresourceRange = *m_pSubresourceRange;

			VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;			// after transfer stage
			VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;	// before final stage
			vkCmdPipelineBarrier(
				command.GetVkCommandBuffer(),
				srcStage, dstStage,
				0,	// dependency flags, typically 0
				0, nullptr,						// memory barriers
				0, nullptr,			// buffer memory barriers
				1, &releaseBarrier);	// image memory barriers

			VulkanCommand::EndSingleTimeCommand(m_pContext, command, m_pContext->pLogicalDevice->GetTransferQueue());

			m_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		{
			VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(m_pContext, m_pContext->pLogicalDevice->GetGraphicsQueue());

			VkImageMemoryBarrier acquireBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
			acquireBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			acquireBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			acquireBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			acquireBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			acquireBarrier.srcQueueFamilyIndex = m_pContext->pLogicalDevice->GetTransferQueue().familyIndex;
			acquireBarrier.dstQueueFamilyIndex = m_pContext->pLogicalDevice->GetGraphicsQueue().familyIndex;
			acquireBarrier.image = m_image;
			acquireBarrier.subresourceRange = *m_pSubresourceRange;

			VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;			// after transfer stage
			VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;	// before fragment shader stage
			vkCmdPipelineBarrier(
				command.GetVkCommandBuffer(),
				srcStage, dstStage,
				0,	// dependency flags, typically 0
				0, nullptr,						// memory barriers
				0, nullptr,			// buffer memory barriers
				1, &acquireBarrier);	// image memory barriers

			VulkanCommand::EndSingleTimeCommand(m_pContext, command, m_pContext->pLogicalDevice->GetGraphicsQueue());

			m_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
	}
	void VmaImage::GenerateMipmaps(uint32_t mipLevels)
	{
		VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(m_pContext, m_pContext->pLogicalDevice->GetGraphicsQueue());

		VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.image = m_image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int mipWidth = static_cast<int>(GetWidth());
		int mipHeight = static_cast<int>(GetHeight());
		int mipDepth = static_cast<int>(GetDepth());
		for (uint32_t i = 1; i < mipLevels; i++)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(command.GetVkCommandBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,				// memory barriers
				0, nullptr,	// buffer memory barrier
				1, &barrier);	// image memory barriers

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, mipDepth };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(command.GetVkCommandBuffer(),
				m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(command.GetVkCommandBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			mipWidth = std::max(1, mipWidth / 2);
			mipHeight = std::max(1, mipHeight / 2);
			mipDepth = std::max(1, mipDepth / 2);
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(command.GetVkCommandBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VulkanCommand::EndSingleTimeCommand(m_pContext, command, m_pContext->pLogicalDevice->GetGraphicsQueue());

		m_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}



	// Static methods:
	void VmaImage::CopyImageToImage(VulkanContext* m_pContext, VmaImage* srcImage, VmaImage* dstImage, const VulkanQueue& queue)
	{
		VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(m_pContext, queue);;

		// Queue copy command:
		VkImageCopy copyRegion = {};
		copyRegion.srcSubresource = srcImage->GetSubresourceLayers();
		copyRegion.srcOffset = { 0, 0, 0 };
		copyRegion.dstSubresource = dstImage->GetSubresourceLayers();
		copyRegion.dstOffset = { 0, 0, 0 };
		copyRegion.extent = srcImage->GetExtent();
		vkCmdCopyImage(command.GetVkCommandBuffer(), srcImage->GetVkImage(), srcImage->GetLayout(), dstImage->GetVkImage(), dstImage->GetLayout(), 1, &copyRegion);

		VulkanCommand::EndSingleTimeCommand(m_pContext, command, queue);
	}
}