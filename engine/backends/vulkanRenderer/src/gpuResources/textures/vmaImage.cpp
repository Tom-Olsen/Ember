#include "vmaImage.h"
#include "vulkanAccessMasks.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStages.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanStagingBuffer.h"



namespace vulkanRendererBackend
{
	// Static private members:
	uint32_t VmaImage::s_index = 0;



	// Constructors/Destructor:
	VmaImage::VmaImage(const std::string name, const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo, VkImageSubresourceRange& subresourceRange, VkImageViewType viewType, const DeviceQueue& queue)
	{
		m_name = name + std::to_string(s_index);
		s_index++;
		m_imageInfo = imageInfo;
		m_allocationInfo = allocationInfo;
		m_subresourceRange = subresourceRange;
		m_queue = queue;
		m_layout = m_imageInfo.initialLayout;

		// Create image:
		VKA(vmaCreateImage(Context::GetVmaAllocator(), &m_imageInfo, &m_allocationInfo, &m_image, &m_allocation, nullptr));

		// Create image view:
		VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewInfo.image = m_image;
		viewInfo.viewType = viewType;
		viewInfo.format = m_imageInfo.format;
		viewInfo.subresourceRange = m_subresourceRange;
		VKA(vkCreateImageView(Context::GetVkDevice(), &viewInfo, nullptr, &m_imageView));

		#ifdef VALIDATION_LAYERS_ACTIVE
		Context::allocationTracker.AddVmaImage(this);
		#endif
	}
	VmaImage::~VmaImage()
	{
		Cleanup();
		#ifdef VALIDATION_LAYERS_ACTIVE
		Context::allocationTracker.RemoveVmaImage(this);
		#endif
	}



	// Move semantics:
	VmaImage::VmaImage(VmaImage&& other) noexcept
	{
		MoveFrom(other);
	}
	VmaImage& VmaImage::operator=(VmaImage&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
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
	const DeviceQueue& VmaImage::GetDeviceQueue() const
	{
		return m_queue;
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
	void VmaImage::TransitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		VkImageMemoryBarrier2 barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
		barrier.srcStageMask = srcStage;
		barrier.dstStageMask = dstStage;
		barrier.srcAccessMask = srcAccessMask;
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

		vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

		m_layout = newLayout;
	}
	void VmaImage::TransitionLayout(VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(m_queue);
		TransitionLayout(commandBuffer, newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		SingleTimeCommand::EndCommand(m_queue);
	}
	void VmaImage::GenerateMipmaps(VkCommandBuffer commandBuffer, uint32_t mipLevels)
	{
        // Memory barrier to ensure transfer queue is done with image upload before mipmapping:
        {
            VkImageMemoryBarrier2 barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
            barrier.srcStageMask = pipelineStage::transfer; // before was upload which is a transfer.
            barrier.dstStageMask = pipelineStage::transfer; // next is blitting, which belongs to transfer.
            barrier.srcAccessMask = accessMask::transfer::transferWrite;
            barrier.dstAccessMask = accessMask::transfer::transferWrite;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = m_image;
            barrier.subresourceRange = m_subresourceRange;

            VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
            dependencyInfo.imageMemoryBarrierCount = 1;
            dependencyInfo.pImageMemoryBarriers = &barrier;

            vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
        }

		int mipWidth = static_cast<int>(GetWidth());
		int mipHeight = static_cast<int>(GetHeight());
		int mipDepth = static_cast<int>(GetDepth());
		for (uint32_t i = 1; i < mipLevels; i++)
		{
			// Perform blit from mip level i-1 to mip level i:
			{
				VkImageMemoryBarrier2 barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
				barrier.srcStageMask = pipelineStage::transfer;
				barrier.dstStageMask = pipelineStage::transfer;
				barrier.srcAccessMask = accessMask::transfer::transferWrite;
				barrier.dstAccessMask = accessMask::transfer::transferRead;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.image = m_image;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.subresourceRange.layerCount = 1;
				barrier.subresourceRange.levelCount = 1;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.imageMemoryBarrierCount = 1;
				dependencyInfo.pImageMemoryBarriers = &barrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

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

				vkCmdBlitImage(commandBuffer,
					m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);
			}

			// Transition mip level i-1 from TRANSFER_SRC_OPTIMAL to SHADER_READ_ONLY_OPTIMAL for shader access:
			{
				VkImageMemoryBarrier2 barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
				barrier.srcStageMask = pipelineStage::transfer;
				barrier.dstStageMask = pipelineStage::fragmentShader;
				barrier.srcAccessMask = accessMask::transfer::transferRead;
				barrier.dstAccessMask = accessMask::fragmentShader::shaderRead;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.image = m_image;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.subresourceRange.layerCount = 1;
				barrier.subresourceRange.levelCount = 1;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.imageMemoryBarrierCount = 1;
				dependencyInfo.pImageMemoryBarriers = &barrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
			}

            // Update dimensions for next mipmap:
			mipWidth = std::max(1, mipWidth / 2);
			mipHeight = std::max(1, mipHeight / 2);
			mipDepth = std::max(1, mipDepth / 2);
		}

		// Transition last mip level from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to SHADER_READ_ONLY_OPTIMAL:
		{
			VkImageMemoryBarrier2 barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
			barrier.srcStageMask = pipelineStage::transfer;
			barrier.dstStageMask = pipelineStage::fragmentShader;
			barrier.srcAccessMask = accessMask::transfer::transferWrite;
			barrier.dstAccessMask = accessMask::fragmentShader::shaderRead;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = m_image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.baseMipLevel = mipLevels - 1;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;

			VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
			dependencyInfo.imageMemoryBarrierCount = 1;
			dependencyInfo.pImageMemoryBarriers = &barrier;

			vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
		}
		m_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}
	void VmaImage::GenerateMipmaps(uint32_t mipLevels)
	{
		VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(Context::logicalDevice.GetGraphicsQueue());
		GenerateMipmaps(commandBuffer, mipLevels);
		SingleTimeCommand::EndCommand(Context::logicalDevice.GetGraphicsQueue());
	}



	// Static methods:
	void VmaImage::CopyImageToImage(VkCommandBuffer commandBuffer, VmaImage* srcImage, VmaImage* dstImage, const DeviceQueue& queue)
	{
		VkImageCopy copyRegion = {};
		copyRegion.srcSubresource = srcImage->GetSubresourceLayers();
		copyRegion.srcOffset = { 0, 0, 0 };
		copyRegion.dstSubresource = dstImage->GetSubresourceLayers();
		copyRegion.dstOffset = { 0, 0, 0 };
		copyRegion.extent = srcImage->GetExtent();
		vkCmdCopyImage(commandBuffer, srcImage->GetVkImage(), srcImage->GetLayout(), dstImage->GetVkImage(), dstImage->GetLayout(), 1, &copyRegion);
	}
	void VmaImage::CopyImageToImage(VmaImage* srcImage, VmaImage* dstImage, const DeviceQueue& queue)
	{
		VkCommandBuffer commandBuffer = SingleTimeCommand::BeginCommand(queue);
        CopyImageToImage(commandBuffer, srcImage, dstImage, queue);
		SingleTimeCommand::EndCommand(queue);
	}



	// Private methods:
	void VmaImage::Cleanup()
	{
		vmaDestroyImage(Context::GetVmaAllocator(), m_image, m_allocation);
		vkDestroyImageView(Context::GetVkDevice(), m_imageView, nullptr);
	}
	void VmaImage::MoveFrom(VmaImage& other) noexcept
	{
		m_name = other.m_name;
		m_image = other.m_image;
		m_allocation = other.m_allocation;
		m_imageView = other.m_imageView;
		m_imageInfo = other.m_imageInfo;
		m_allocationInfo = other.m_allocationInfo;
		m_subresourceRange = other.m_subresourceRange;
		m_queue = other.m_queue;
		m_layout = other.m_layout;

		other.m_name = "";
		other.m_image = VK_NULL_HANDLE;
		other.m_allocation = nullptr;
		other.m_imageView = VK_NULL_HANDLE;
		other.m_imageInfo = {};
		other.m_allocationInfo = {};
		other.m_subresourceRange = {};
		other.m_queue = {};
		other.m_layout = VK_IMAGE_LAYOUT_MAX_ENUM;
	}
}