#pragma once
#include "emberMath.h"
#include "vulkanAccessMask.h"
#include "vulkanDescriptorSetBindingHandle.h"
#include "vulkanShaderHandle.h"
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;



	enum class SceneColorBindingMode : uint8_t
	{
		none,
		inPlace,
		outOfPlace
	};



	struct ComputeCall
	{
		Uint3 threadCount;													// Total thread count in each dimension. GroupCount is automatically computed from blockSize of the ComputeShader.
		ShaderHandle computeShaderHandle;									// Empty for barrier calls.
		DescriptorSetBindingHandle callDescriptorSetBindingHandle;			// Borrowed from pool for dispatch calls, empty for barriers.
		AccessMask srcAccessMask;											// Only applies to barriers.
		AccessMask dstAccessMask;											// Only applies to barriers.
		SceneColorBindingMode sceneColorBindingMode = SceneColorBindingMode::none;	// Determines how this call automatically binds scene color textures.
		bool useRenderTextureSize = false; 											// Determines whether this call provides its own threadCount or uses the render texture size.

		ComputeShader* GetComputeShader() const;
		bool IsBarrier() const;
		std::string ToString() const;
	};
}