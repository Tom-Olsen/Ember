#include "vmaImage.h"
#include "timer.h"
#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Static private members:
	uint32_t VmaImage::s_index = 0;



	// Constructors/Destructor:
	VmaImage::VmaImage(const std::string name, const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo, VkImageSubresourceRange& subresourceRange, VkImageViewType viewType, const VulkanQueue& queue)
	{
		m_name = name + std::to_string(s_index);
		s_index++;
		m_imageInfo = imageInfo;
		m_allocationInfo = allocationInfo;
		m_subresourceRange = subresourceRange;
		m_queue = queue;
		m_layout = m_imageInfo.initialLayout;

		// Create image:
		VKA(vmaCreateImage(VulkanContext::GetVmaAllocator(), &m_imageInfo, &m_allocationInfo, &m_image, &m_allocation, nullptr));

		// Create image view:
		VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewInfo.image = m_image;
		viewInfo.viewType = viewType;
		viewInfo.format = m_imageInfo.format;
		viewInfo.subresourceRange = m_subresourceRange;
		VKA(vkCreateImageView(VulkanContext::GetVkDevice(), &viewInfo, nullptr, &m_imageView));

		#ifdef VALIDATION_LAYERS_ACTIVE
		VulkanContext::pAllocationTracker->AddVmaImage(this);
		#endif
	}
	VmaImage::~VmaImage()
	{
		vmaDestroyImage(VulkanContext::GetVmaAllocator(), m_image, m_allocation);
		vkDestroyImageView(VulkanContext::GetVkDevice(), m_imageView, nullptr);

		#ifdef VALIDATION_LAYERS_ACTIVE
		VulkanContext::pAllocationTracker->RemoveVmaImage(this);
		#endif
	}



	// Public methods:
	// Getters:
	const std::string& VmaImage::GetName() const
	{
		return m_name;
	}
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
	const VkImageCreateInfo& VmaImage::GetVkImageCreateInfo() const
	{
		return m_imageInfo;
	}
	const VmaAllocationCreateInfo& VmaImage::GetVmaAllocationCreateInfo() const
	{
		return m_allocationInfo;
	}
	const VkImageSubresourceRange& VmaImage::GetSubresourceRange() const
	{
		return m_subresourceRange;
	}
	const VkImageLayout& VmaImage::GetLayout() const
	{
		return m_layout;
	}
	uint64_t VmaImage::GetWidth() const
	{
		return m_imageInfo.extent.width;
	}
	uint64_t VmaImage::GetHeight() const
	{
		return m_imageInfo.extent.height;
	}
	uint64_t VmaImage::GetDepth() const
	{
		return m_imageInfo.extent.depth;
	}
	const VkExtent3D& VmaImage::GetExtent() const
	{
		return m_imageInfo.extent;
	}
	VkFormat VmaImage::GetFormat() const
	{
		return m_imageInfo.format;
	}
	VkImageSubresourceLayers VmaImage::GetSubresourceLayers() const
	{
		VkImageSubresourceLayers subresourceLayers = {};
		subresourceLayers.aspectMask = m_subresourceRange.aspectMask;
		subresourceLayers.mipLevel = m_subresourceRange.baseMipLevel;
		subresourceLayers.baseArrayLayer = m_subresourceRange.baseArrayLayer;
		subresourceLayers.layerCount = m_subresourceRange.layerCount;
		return subresourceLayers;
	}



	// Setters:
	void VmaImage::SetLayout(VkImageLayout imageLayout)
	{
		m_layout = imageLayout;
	}



	// Transitions:
	// Notes:
	// - srcStage = flag							<->		this stage must finish before barrier blocks.
	// - dstStage = flag							<->		this stage must wait for the barrier to release.
	// - barrier.srcAccessMask = multiple flags		<->		these memory accesses must be flushed to L2 cache before barrier blocks.
	// - barrier.dstAccessMask = multiple flags		<->		these memory accesses must wait for the barrier before accessing L2 cache.
	void VmaImage::TransitionLayout(VkCommandBuffer vkCommandBuffer, VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		VkImageMemoryBarrier2 barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
		barrier.srcStageMask = srcStage;
		barrier.srcAccessMask = srcAccessMask;
		barrier.dstStageMask = dstStage;
		barrier.dstAccessMask = dstAccessMask;
		barrier.oldLayout = m_layout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_image;
		barrier.subresourceRange = m_subresourceRange;

		VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
		dependencyInfo.imageMemoryBarrierCount = 1;
		dependencyInfo.pImageMemoryBarriers = &barrier;

		vkCmdPipelineBarrier2(vkCommandBuffer, &dependencyInfo);

		m_layout = newLayout;
	}
	void VmaImage::TransitionLayout(VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		// Only transition layout. Queue remains unchanged.
		VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(m_queue);

		VkImageMemoryBarrier2 barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
		barrier.srcStageMask = srcStage;
		barrier.srcAccessMask = srcAccessMask;
		barrier.dstStageMask = dstStage;
		barrier.dstAccessMask = dstAccessMask;
		barrier.oldLayout = m_layout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_image;
		barrier.subresourceRange = m_subresourceRange;

		VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
		dependencyInfo.imageMemoryBarrierCount = 1;
		dependencyInfo.pImageMemoryBarriers = &barrier;

		vkCmdPipelineBarrier2(command.GetVkCommandBuffer(), &dependencyInfo);

		VulkanCommand::EndSingleTimeCommand(command, m_queue);

		m_layout = newLayout;
	}
	void VmaImage::TransitionQueue(const VulkanQueue& newQueue, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		VkImageLayout newLayout = m_layout;	// new layout = old layout
		TransitionLayoutAndQueue(newLayout, newQueue, srcStage, dstStage, srcAccessMask, dstAccessMask);
	}
	void VmaImage::TransitionLayoutAndQueue(VkImageLayout newLayout, const VulkanQueue& newQueue, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		// Barrier on release queue:
		VulkanCommand releaseCommand = VulkanCommand::BeginSingleTimeCommand(m_queue);

		VkImageMemoryBarrier2 releaseBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
		releaseBarrier.srcStageMask = srcStage;
		releaseBarrier.srcAccessMask = srcAccessMask;
		releaseBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
		releaseBarrier.dstAccessMask = VK_ACCESS_2_NONE;
		releaseBarrier.oldLayout = m_layout;
		releaseBarrier.newLayout = newLayout;
		releaseBarrier.srcQueueFamilyIndex = m_queue.familyIndex;
		releaseBarrier.dstQueueFamilyIndex = newQueue.familyIndex;
		releaseBarrier.image = m_image;
		releaseBarrier.subresourceRange = m_subresourceRange;

		VkDependencyInfo releaseDependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
		releaseDependencyInfo.imageMemoryBarrierCount = 1;
		releaseDependencyInfo.pImageMemoryBarriers = &releaseBarrier;

		vkCmdPipelineBarrier2(releaseCommand.GetVkCommandBuffer(), &releaseDependencyInfo);
		VulkanCommand::EndSingleTimeCommand(releaseCommand, m_queue);


		// Barrier on receiving queue:
		VulkanCommand receiveCommand = VulkanCommand::BeginSingleTimeCommand(newQueue);

		VkImageMemoryBarrier2 receiveBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
		receiveBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		receiveBarrier.srcAccessMask = VK_ACCESS_2_NONE;
		receiveBarrier.dstStageMask = dstStage;
		receiveBarrier.dstAccessMask = dstAccessMask;
		receiveBarrier.oldLayout = m_layout;
		receiveBarrier.newLayout = newLayout;
		receiveBarrier.srcQueueFamilyIndex = m_queue.familyIndex;
		receiveBarrier.dstQueueFamilyIndex = newQueue.familyIndex;
		receiveBarrier.image = m_image;
		receiveBarrier.subresourceRange = m_subresourceRange;

		VkDependencyInfo receiveDependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
		receiveDependencyInfo.imageMemoryBarrierCount = 1;
		receiveDependencyInfo.pImageMemoryBarriers = &receiveBarrier;

		vkCmdPipelineBarrier2(receiveCommand.GetVkCommandBuffer(), &receiveDependencyInfo);
		VulkanCommand::EndSingleTimeCommand(receiveCommand, newQueue);

		m_layout = newLayout;
		m_queue = newQueue;
	}
	void VmaImage::GenerateMipmaps(uint32_t mipLevels)
	{
		VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(VulkanContext::pLogicalDevice->GetGraphicsQueue());

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

			// Transition finished mip level to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: (synced at fragment shader stage)
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

		VulkanCommand::EndSingleTimeCommand(command, VulkanContext::pLogicalDevice->GetGraphicsQueue());

		m_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}



	// Static methods:
	void VmaImage::CopyImageToImage(VmaImage* srcImage, VmaImage* dstImage, const VulkanQueue& queue)
	{
		VulkanCommand command = VulkanCommand::BeginSingleTimeCommand(queue);;

		VkImageCopy copyRegion = {};
		copyRegion.srcSubresource = srcImage->GetSubresourceLayers();
		copyRegion.srcOffset = { 0, 0, 0 };
		copyRegion.dstSubresource = dstImage->GetSubresourceLayers();
		copyRegion.dstOffset = { 0, 0, 0 };
		copyRegion.extent = srcImage->GetExtent();
		vkCmdCopyImage(command.GetVkCommandBuffer(), srcImage->GetVkImage(), srcImage->GetLayout(), dstImage->GetVkImage(), dstImage->GetLayout(), 1, &copyRegion);

		VulkanCommand::EndSingleTimeCommand(command, queue);
	}
	void VmaImage::CopyImageToImage(VkCommandBuffer commandBuffer, VmaImage* srcImage, VmaImage* dstImage, const VulkanQueue& queue)
	{
		VkImageCopy copyRegion = {};
		copyRegion.srcSubresource = srcImage->GetSubresourceLayers();
		copyRegion.srcOffset = { 0, 0, 0 };
		copyRegion.dstSubresource = dstImage->GetSubresourceLayers();
		copyRegion.dstOffset = { 0, 0, 0 };
		copyRegion.extent = srcImage->GetExtent();
		vkCmdCopyImage(commandBuffer, srcImage->GetVkImage(), srcImage->GetLayout(), dstImage->GetVkImage(), dstImage->GetLayout(), 1, &copyRegion);
	}
}