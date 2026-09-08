#pragma once
#include "vulkanComputeQueue.h"
#include "vulkanRendererExport.h"
#include <cstdint>



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IDescriptorSetBinding;
}
namespace emberSpirvReflect
{
	class DescriptorReflection;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;
	class SceneColorTexture2dPair;
	enum class PostProcessingMode : uint8_t;



	class VULKAN_RENDERER_API PostRenderComputeQueue : public ComputeQueue, public emberBackendInterface::ICompute::IPostRenderQueue
	{
	public: // Methods:
		// Constructor/Destructor:
		PostRenderComputeQueue();
		~PostRenderComputeQueue() override;

		// Non-copyable:
		PostRenderComputeQueue(const PostRenderComputeQueue&) = delete;
		PostRenderComputeQueue& operator=(const PostRenderComputeQueue&) = delete;

		// Movable:
		PostRenderComputeQueue(PostRenderComputeQueue&& other) noexcept;
		PostRenderComputeQueue& operator=(PostRenderComputeQueue&& other) noexcept;

		// Workload recording:
		emberBackendInterface::IDescriptorSetBinding* RecordPostProcessingShader(emberBackendInterface::IComputeShader* pComputeShader, Uint3 threadCount) override;

		// Management:
		void UpdateShaderData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair);

	private: // Methods:
		PostProcessingMode DeterminePostProcessingMode(const ComputeShader& computeShader) const;
		void ValidatePostProcessingImage(const ComputeShader& computeShader, const emberSpirvReflect::DescriptorReflection& descriptorReflection, bool allowSampledImage, bool requireReadable, bool requireWritable) const;
	};
}