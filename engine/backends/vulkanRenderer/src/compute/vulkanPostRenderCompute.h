#pragma once
#include "iCompute.h"
#include "vulkanRendererExport.h"
#include <cstddef>
#include <cstdint>
#include <vector>



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IDescriptorSetBinding;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;
	struct ComputeCall;



	class VULKAN_RENDERER_API PostRender : public emberBackendInterface::ICompute::IPostRender
	{
	private: // Members:
		std::vector<ComputeCall> m_computeCalls;
		std::vector<std::vector<ComputeCall>> m_submittedComputeCalls;
		size_t m_postProcessingCallCount;

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
		size_t GetPostProcessingCallCount() const;
		void ResetComputeCalls();

	private: // Methods:
		emberBackendInterface::IDescriptorSetBinding* RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader, bool isPostProcessing);
		void ReleaseComputeCalls(std::vector<ComputeCall>& computeCalls);
	};
}