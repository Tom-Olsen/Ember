#pragma once
#include "emberMath.h"
#include "vulkanFormat.h"
#include "vulkanImageCreateFlag.h"
#include "vulkanImageLayout.h"
#include "vulkanImageTiling.h"
#include "vulkanImageType.h"
#include "vulkanImageUsageFlag.h"
#include "vulkanSampleCountFlag.h"
#include "vulkanSharingMode.h"
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	struct ImageCreateInfo
	{
		const void* pNext;
		ImageCreateFlag flags;
		ImageType imageType;
		Format format;
		Uint3 extent;
		uint32_t mipLevels;
		uint32_t arrayLayers;
		SampleCountFlag sampleCountFlags;
		ImageTiling tiling;
		ImageUsageFlag usages;
		SharingMode sharingMode;
		uint32_t queueFamilyIndexCount;
		const uint32_t* pQueueFamilyIndices;
		ImageLayout initialLayout;
		std::string ToString() const;
	};
}