#ifndef __INCLUDE_GUARD_computeCall_h__
#define __INCLUDE_GUARD_computeCall_h__
#include "emberMath.h"
#include <array>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class ComputeShader;
	class ShaderProperties;

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