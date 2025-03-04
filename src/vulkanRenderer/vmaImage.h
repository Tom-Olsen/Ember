#ifndef __INCLUDE_GUARD_vmaImage_h__
#define __INCLUDE_GUARD_vmaImage_h__
#include "vk_mem_alloc.h"
#include "vulkanQueue.h"
#include <memory>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	/// <summary>
	/// Manages VkImage and VkImageView resource: creation, allocation, destruction, queue ownership, layout.
	/// Usually further abstracted by texture class which wraps around it.
	/// Sometimes used stand alone when the image is strictly used for a specific usecase,
	/// e.g. msaa color and depth attachment images of forward renderpass.
	/// </summary>
	class VmaImage
	{
	private: // Members:
		VkImage m_image;
		VmaAllocation m_allocation;
		VkImageView m_imageView;
		VkImageCreateInfo m_imageInfo;
		VmaAllocationCreateInfo m_allocationInfo;
		VkImageSubresourceRange m_subresourceRange;
		VulkanQueue m_queue;
		VkImageLayout m_layout;

	public: // Methods:
		VmaImage(const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo, VkImageSubresourceRange& subresourceRange, VkImageViewType viewType, const VulkanQueue& queue);
		~VmaImage();


		// Getters:
		const VkImage& GetVkImage() const;
		const VmaAllocation& GetVmaAllocation() const;
		const VkImageView& GetVkImageView() const;
		const VkImageCreateInfo& GetVkImageCreateInfo() const;
		const VmaAllocationCreateInfo& GetVmaAllocationCreateInfo() const;
		const VkImageSubresourceRange& GetSubresourceRange() const;
		const VkImageLayout& GetLayout() const;
		uint64_t GetWidth() const;
		uint64_t GetHeight() const;
		uint64_t GetDepth() const;
		const VkExtent3D& GetExtent() const;
		VkImageSubresourceLayers GetSubresourceLayers() const;

		// Setters:
		// Only changes the m_layout member without doing an image layout transition.
		void SetLayout(VkImageLayout imageLayout);
		
		// Transitions:
		void TransitionLayout(VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask);
		void TransitionQueue(const VulkanQueue& newQueue, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask);
		void TransitionLayoutAndQueue(VkImageLayout newLayout, const VulkanQueue& newQueue, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask);
		void GenerateMipmaps(uint32_t mipLevels);

		// Static methods:
		static void CopyImageToImage(VmaImage* srcImage, VmaImage* dstImage, const VulkanQueue& queue);
	};
}



#endif // __INCLUDE_GUARD_vmaImage_h__