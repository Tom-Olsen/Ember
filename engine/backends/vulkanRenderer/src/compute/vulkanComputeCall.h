#pragma once
#include "emberMath.h"
#include "vulkanAccessMask.h"
#include "vulkanDescriptorSetBindingHandle.h"
#include "vulkanShaderHandle.h"
#include <string>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;



	struct ComputeCall
	{
		Uint3 threadCount;											// Total thread count in each dimension. GroupCount is automatically computed from blockSize of the ComputeShader.
		ShaderHandle computeShaderHandle;							// Empty for barrier calls.
		DescriptorSetBindingHandle callDescriptorSetBindingHandle;	// Borrowed from pool for dispatch calls, empty for barriers.
		AccessMask srcAccessMask;									// Only applies to barriers.
		AccessMask dstAccessMask;									// Only applies to barriers.
		bool isPostProcessing = false;								// Uses the renderer-managed input/output image ping-pong chain.

		ComputeShader* GetComputeShader() const;
		bool IsBarrier() const;
		std::string ToString() const;
	};
}