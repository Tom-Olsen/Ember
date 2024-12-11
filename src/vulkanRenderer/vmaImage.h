#ifndef __INCLUDE_GUARD_vulkanImage_h__
#define __INCLUDE_GUARD_vulkanImage_h__
#include "vk_mem_alloc.h"
#include <memory>
#include <vulkan/vulkan.h>



struct VulkanContext;
struct VulkanQueue;



class VmaImage
{
private: // Members:
	VkImage m_image;
	VmaAllocation m_allocation;
	VkImageView m_imageView;
	std::unique_ptr<VkImageCreateInfo> m_pImageInfo;
	std::unique_ptr<VmaAllocationCreateInfo> m_pAllocationInfo;
	std::unique_ptr<VkImageSubresourceRange> m_pSubresourceRange;
	VkImageLayout m_layout;
	VulkanContext* m_pContext;

public: // Methods:
	VmaImage(VulkanContext* pContext, VkImageCreateInfo* pImageInfo, VmaAllocationCreateInfo* pAllocationInfo, VkImageSubresourceRange* pSubresourceRange);
	~VmaImage();

	// Getters:
	const VkImage& GetVkImage() const;
	const VmaAllocation& GetVmaAllocation() const;
	const VkImageView& GetVkImageView() const;
	const VkImageCreateInfo* const GetVkImageCreateInfo() const;
	const VmaAllocationCreateInfo* const GetVmaAllocationCreateInfo() const;
	const VkImageSubresourceRange* const GetSubresourceRange() const;
	const VkImageLayout& GetLayout() const;
	uint64_t GetWidth() const;
	uint64_t GetHeight() const;
	uint64_t GetDepth() const;
	const VkExtent3D& GetExtent() const;
	VkImageSubresourceLayers GetSubresourceLayers() const;

	// Transitions etc.:
	void TransitionLayoutUndefinedToTransfer();
	void HandoffTransferToGraphicsQueue();
	void TransitionLayoutTransferToShaderRead();
	void GenerateMipmaps(uint32_t mipLevels);

	// Static methods:
	static void CopyImageToImage(VulkanContext* context, VmaImage* srcImage, VmaImage* dstImage, const VulkanQueue& queue);
};



#endif // __INCLUDE_GUARD_vulkanImage_h__