#ifndef __INCLUDE_GUARD_vulkanImage_h__
#define __INCLUDE_GUARD_vulkanImage_h__
#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>



struct VulkanContext;
struct VulkanQueue;



class VmaImage
{
private: // Members:
	VkImage m_image;
	VmaAllocation m_allocation;
	VkImageView m_imageView;
	VkImageCreateInfo m_imageInfo;
	VmaAllocationCreateInfo m_allocationInfo;
	VkImageSubresourceRange m_subresourceRange;
	VkImageLayout m_layout;
	VulkanContext* m_pContext;

public: // Methods:
	VmaImage(VulkanContext* pContext, const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo, const VkImageSubresourceRange& subresourceRange);
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

	// Transitions etc.:
	// TODO: remove the subresourceRange parameter from all of these functions
	void TransitionLayoutUndefinedToTransfer();
	void HandoffTransferToGraphicsQueue();
	void TransitionLayoutTransferToShaderRead();
	void GenerateMipmaps(uint32_t mipLevels);

	// Static methods:
	static void CopyImageToImage(VulkanContext* context, VmaImage* srcImage, VmaImage* dstImage, const VulkanQueue& queue);
};



#endif // __INCLUDE_GUARD_vulkanImage_h__