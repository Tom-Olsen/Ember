#pragma once
#include "vk_mem_alloc.h"
#include <string>



namespace vulkanRendererBackend
{
	std::string ToString_VmaAllocationCreateFlags(VmaAllocationCreateFlags vmaAllocationCreateFlags);
}