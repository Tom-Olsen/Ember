#ifndef __INCLUDE_GUARD_computeCall_h__
#define __INCLUDE_GUARD_computeCall_h__
#include "macros.h"
#include "mathf.h"
#include <array>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;

	namespace ComputeStageAccessMask
	{
		constexpr VkAccessFlagBits2 uniformRead      = VK_ACCESS_2_UNIFORM_READ_BIT;
		constexpr VkAccessFlagBits2 shaderRead       = VK_ACCESS_2_SHADER_READ_BIT;		// includes all read operations!
		constexpr VkAccessFlagBits2 shaderWrite      = VK_ACCESS_2_SHADER_WRITE_BIT;	// includes all write operations!
		constexpr VkAccessFlagBits2 sampleRead       = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
		constexpr VkAccessFlagBits2 storageRead      = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
		constexpr VkAccessFlagBits2 storageWrite     = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
		constexpr VkAccessFlagBits2 accelerationRead = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
		constexpr VkAccessFlagBits2 descrBufferRead  = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT;
	}

	struct ComputeCall
	{
		uint32_t callIndex;	// Tracks execution order.
		Uint3 threadCount;	// Total thread count in each dimension. GroupCount is automatically computed from blockSize of the ComputeShader.
		ComputeShader* pComputeShader;			// If null => barrier call.
		ShaderProperties* pShaderProperties;	// Supplied by caller (static), or borrowed from pool (dynamic).
		VkAccessFlags2 srcAccessMask;			// Only apply to barriers.
		VkAccessFlags2 dstAccessMask;			// Only apply to barriers.	
	};
}



#endif // __INCLUDE_GUARD_computeCall_h__