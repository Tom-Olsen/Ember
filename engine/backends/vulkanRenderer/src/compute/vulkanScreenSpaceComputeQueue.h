#pragma once
#include "vulkanComputeQueue.h"
#include "vulkanRendererExport.h"
#include <cstdint>



// Forward declarations:
namespace emberSpirvReflect
{
	class DescriptorReflection;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;
	class DepthTexture2d;
	class SceneColorTexture2dPair;
	enum class SceneColorBindingMode : uint8_t;



	class VULKAN_RENDERER_API ScreenSpaceComputeQueue : public ComputeQueue
	{
	public: // Methods:
		// Constructor/Destructor:
		ScreenSpaceComputeQueue();
		~ScreenSpaceComputeQueue() override;

		// Non-copyable:
		ScreenSpaceComputeQueue(const ScreenSpaceComputeQueue&) = delete;
		ScreenSpaceComputeQueue& operator=(const ScreenSpaceComputeQueue&) = delete;

		// Movable:
		ScreenSpaceComputeQueue(ScreenSpaceComputeQueue&& other) noexcept;
		ScreenSpaceComputeQueue& operator=(ScreenSpaceComputeQueue&& other) noexcept;

		// Workload recording:
		emberBackendInterface::IDescriptorSetBinding* RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader, Uint3 threadCount) override;

		// Management:
		void UpdateShaderData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair, DepthTexture2d& sceneDepth);

	private: // Methods:
		SceneColorBindingMode DetermineSceneColorBindingMode(const ComputeShader& computeShader) const;
		void ValidateSceneColorImage(const ComputeShader& computeShader, const emberSpirvReflect::DescriptorReflection& descriptorReflection, bool allowSampledImage, bool requireReadable, bool requireWritable) const;
		void ValidateSceneDepth(const ComputeShader& computeShader) const;
	};
}