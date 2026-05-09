#pragma once
#include "vk_mem_alloc.h"
#include <string>



namespace vulkanRendererBackend
{
	std::string ToString_VmaAllocationCreateInfo(const VmaAllocationCreateInfo& vmaAllocationCreateInfo);
}