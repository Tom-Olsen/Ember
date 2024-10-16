#pragma once
#ifndef __INCLUDE_GUARD_vulkanImage_h__
#define __INCLUDE_GUARD_vulkanImage_h__
#include "vulkanContext.h"



class VmaImage
{
public: // Members:
	VkImage image;
	VmaAllocation allocation;
	VkImageView imageView;

private: // Members:
	VulkanContext* context;
	VkImageCreateInfo imageInfo;
	VmaAllocationCreateInfo allocationInfo;
	VkImageSubresourceRange subresourceRange;

public: // Constructor/Destructor:
	VmaImage(VulkanContext* context, const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo, const VkImageSubresourceRange& subresourceRange);
	~VmaImage();

public: // Getters:
	uint64_t GetWidth();
	uint64_t GetHeight();
	uint64_t GetDepth();

public: // Advanced methods:
	void TransitionLayoutUndefinedToTransfer(VkImageSubresourceRange subresourceRange);
	void HandoffTransferToGraphicsQueue(VkImageSubresourceRange subresourceRange);
	void TransitionLayoutTransferToShaderRead(VkImageSubresourceRange subresourceRange);
	void GenerateMipmaps(uint32_t mipLevels);
};



#endif // __INCLUDE_GUARD_vulkanImage_h__