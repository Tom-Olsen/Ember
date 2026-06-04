#pragma once
#include "emberMath.h"
#include "vulkanAccessMask.h"
#include <string>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;
	class DescriptorSetBinding;



	struct ComputeCall
	{
		Uint3 threadCount;								// Total thread count in each dimension. GroupCount is automatically computed from blockSize of the ComputeShader.
		ComputeShader* pComputeShader;					// If null => barrier call.
		DescriptorSetBinding* pDescriptorSetBinding;	// Borrowed from pool for dispatch calls, null for barriers.
		AccessMask srcAccessMask;						// Only applies to barriers.
		AccessMask dstAccessMask;						// Only applies to barriers.

		std::string ToString() const;
	};
}