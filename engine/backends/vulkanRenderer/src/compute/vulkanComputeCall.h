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
		DescriptorSetBinding* pDescriptorSetBinding;	// Supplied by caller (static), or borrowed from pool (dynamic).
		bool ownsDescriptorSetBinding;					// True if borrowed from pool and returned after execution.
		AccessMask srcAccessMask;						// Only applies to barriers.
		AccessMask dstAccessMask;						// Only applies to barriers.

		std::string ToString() const;
	};
}