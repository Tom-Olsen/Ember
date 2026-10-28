#pragma once
#include "emberMath.h"
#include "vulkanAccessMask.h"
#include "vulkanBufferHandle.h"
#include "vulkanDescriptorSetBindingHandle.h"
#include "vulkanShaderHandle.h"
#include <cstdint>
#include <string>
#include <variant>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;



	struct ComputeCall
	{
	public: // Structs:
		struct BufferCopy
		{
		public: // Members:
			BufferHandle source;
			BufferHandle destination;
			uint64_t size;
			uint64_t sourceOffset;
			uint64_t destinationOffset;
		};
		struct BufferBarrier
		{
		public: // Members:
			BufferHandle buffer;
			VkPipelineStageFlags2 sourceStages;
			AccessMask sourceAccess;
			VkPipelineStageFlags2 destinationStages;
			AccessMask destinationAccess;
			uint64_t offset;
			uint64_t size;
		};

	public: // Members:
		Uint3 threadCount;											// Total thread count in each dimension. GroupCount is automatically computed from blockSize of the ComputeShader.
		ShaderHandle computeShaderHandle;							// Empty for barrier and copy calls.
		DescriptorSetBindingHandle callDescriptorSetBindingHandle;	// Borrowed from pool for dispatch calls, empty for barriers and copies.
		AccessMask srcAccessMask;									// Only applies to barriers.
		AccessMask dstAccessMask;									// Only applies to barriers.
		bool flipSceneColors = false; 								// Determines whether this call writes the alternate scene color texture.
		uint32_t sceneColorIndex = 0;								// Current scene color texture index before this call runs.
		std::variant<std::monostate, BufferCopy, BufferBarrier> bufferCommand;

	public: // Methods:
		// Getters:
		bool IsDispatch() const;
		bool IsBarrier() const;
		bool IsBufferCopy() const;
		bool IsBufferBarrier() const;
		ComputeShader* GetComputeShader() const;
		const BufferCopy& GetBufferCopy() const;
		const BufferBarrier& GetBufferBarrier() const;

		// Debugging:
		std::string ToString() const;
	};
}