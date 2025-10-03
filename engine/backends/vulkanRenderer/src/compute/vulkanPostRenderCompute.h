#pragma once
#include "iCompute.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <vector>



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IShaderProperties;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;
	struct ComputeCall;



	class VULKAN_RENDERER_API PostRender : public emberBackendInterface::ICompute::IPostRender
	{
	private: // Members:
		uint32_t m_callIndex;
		std::vector<ComputeCall> m_staticComputeCalls;
		std::vector<ComputeCall> m_dynamicComputeCalls;
		std::vector<ComputeCall*> m_computeCallPointers;
		std::unique_ptr<ComputeShader> m_pInOutComputeShader; // copies final render into primary texture in case of odd number of post processing effects.

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
		void RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader, emberBackendInterface::IShaderProperties* pShaderProperties) override;
		emberBackendInterface::IShaderProperties* RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader) override;

		// Management:
		std::vector<ComputeCall*>& GetComputeCallPointers();
		void ResetComputeCalls();
	};
}