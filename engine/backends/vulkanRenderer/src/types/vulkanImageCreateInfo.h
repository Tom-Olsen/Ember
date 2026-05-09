#pragma once
#include "emberMath.h"
#include "vulkanFormat.h"
#include <cstdint>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	struct ImageCreateInfo
	{
		const void* pNext;
		VkImageCreateFlags flags;
		VkImageType imageType;
		Format format;
		Uint3 extent;
		uint32_t mipLevels;
		uint32_t arrayLayers;
		VkSampleCountFlagBits sampleCountFlags;
		VkImageTiling tiling;
		VkImageUsageFlags usages;
		VkSharingMode sharingMode;
		uint32_t queueFamilyIndexCount;
		const uint32_t* pQueueFamilyIndices;
		VkImageLayout initialLayout;
		std::string ToString() const;
	};
}