#pragma once
#include "emberMath.h"
#include "vulkanFormat.h"
#include "vulkanImageCreateFlag.h"
#include "vulkanImageUsageFlag.h"
#include <cstdint>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	struct ImageCreateInfo
	{
		const void* pNext;
		ImageCreateFlag flags;
		VkImageType imageType;
		Format format;
		Uint3 extent;
		uint32_t mipLevels;
		uint32_t arrayLayers;
		VkSampleCountFlagBits sampleCountFlags;
		VkImageTiling tiling;
		ImageUsageFlag usages;
		VkSharingMode sharingMode;
		uint32_t queueFamilyIndexCount;
		const uint32_t* pQueueFamilyIndices;
		VkImageLayout initialLayout;
		std::string ToString() const;
	};
}