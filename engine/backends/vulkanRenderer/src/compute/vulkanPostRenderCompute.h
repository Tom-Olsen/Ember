#pragma once
#include "iCompute.h"
#include "vulkanRendererExport.h"
#include <cstdint>
#include <vector>



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
	enum class PostProcessingMode : uint8_t;



	class VULKAN_RENDERER_API PostRender : public emberBackendInterface::ICompute::IPostRender
	{
	private: // Members:
		std::vector<ComputeCall> m_computeCalls;
		std::vector<std::vector<ComputeCall>> m_submittedComputeCalls;

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
		emberBackendInterface::IDescriptorSetBinding* RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader) override;
		emberBackendInterface::IDescriptorSetBinding* RecordPostProcessingShader(emberBackendInterface::IComputeShader* pComputeShader) override;

		// Management:
		void CommitComputeCalls(uint32_t frameIndex);
		void CompleteComputeCalls(uint32_t frameIndex);
		void CompleteAllComputeCalls();
		std::vector<ComputeCall>& GetComputeCalls();
		void ResetComputeCalls();

	private: // Methods:
		emberBackendInterface::IDescriptorSetBinding* RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader, PostProcessingMode postProcessingMode);
		void ReleaseComputeCalls(std::vector<ComputeCall>& computeCalls);
		PostProcessingMode DeterminePostProcessingMode(const ComputeShader& computeShader) const;
		void ValidatePostProcessingImage(const ComputeShader& computeShader, const emberSpirvReflect::DescriptorReflection& descriptorReflection, bool allowSampledImage, bool requireReadable, bool requireWritable) const;
	};
}