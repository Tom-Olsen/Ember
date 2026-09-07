#pragma once
#include "iCompute.h"
#include "vulkanComputeCallQueue.h"
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
	struct ComputeCall;
	class ComputeShader;
	class SceneColorTexture2dPair;
	enum class PostProcessingMode : uint8_t;



	class VULKAN_RENDERER_API PostRender : public emberBackendInterface::ICompute::IPostRender
	{
	private: // Members:
		ComputeCallQueue m_computeCallQueue;

	public: // Methods:
		// Constructor/Destructor:
		PostRender();
		~PostRender();

		// Non-copyable:
		PostRender(const PostRender&) = delete;
		PostRender& operator=(const PostRender&) = delete;

		// Movable:
		PostRender(PostRender&& other) noexcept;
		PostRender& operator=(PostRender&& other) noexcept;

		// Workload recording:
		emberBackendInterface::IDescriptorSetBinding* RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader, Uint3 threadCount) override;
		emberBackendInterface::IDescriptorSetBinding* RecordPostProcessingShader(emberBackendInterface::IComputeShader* pComputeShader, Uint3 threadCount) override;
		void RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags) override;

		// Management:
		void CommitComputeCalls(uint32_t frameIndex);
		void RetireComputeCalls(uint32_t frameIndex);
		void RetireAllComputeCalls();
		std::vector<ComputeCall>& GetComputeCalls();
		void ResetComputeCalls();
		void UpdateShaderData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair);

	private: // Methods:
		emberBackendInterface::IDescriptorSetBinding* RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader, Uint3 threadCount, PostProcessingMode postProcessingMode);
		PostProcessingMode DeterminePostProcessingMode(const ComputeShader& computeShader) const;
		void ValidatePostProcessingImage(const ComputeShader& computeShader, const emberSpirvReflect::DescriptorReflection& descriptorReflection, bool allowSampledImage, bool requireReadable, bool requireWritable) const;
	};
}