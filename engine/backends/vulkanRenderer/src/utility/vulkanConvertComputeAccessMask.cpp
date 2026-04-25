#include "vulkanConvertComputeAccessMask.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	vulkanRendererBackend::AccessMask ComputeAccessFlagsCommonToVulkan(emberCommon::ComputeAccessFlag accessFlags)
	{
		AccessMask accessMask = VK_ACCESS_2_NONE;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::uniformRead))
			accessMask |= VK_ACCESS_2_UNIFORM_READ_BIT;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::shaderRead))
			accessMask |= VK_ACCESS_2_SHADER_READ_BIT;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::shaderWrite))
			accessMask |= VK_ACCESS_2_SHADER_WRITE_BIT;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::sampledRead))
			accessMask |= VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::storageRead))
			accessMask |= VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::storageWrite))
			accessMask |= VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::memoryRead))
			accessMask |= VK_ACCESS_2_MEMORY_READ_BIT;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::memoryWrite))
			accessMask |= VK_ACCESS_2_MEMORY_WRITE_BIT;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::accelerationStructureRead))
			accessMask |= VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
		if (emberCommon::HasFlag(accessFlags, emberCommon::ComputeAccessFlag::descriptorBufferRead))
			accessMask |= VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
		return accessMask;
	}
}