#include "vulkanPostRenderCompute.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanPoolManager.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTexture2d.h"
#include <vulkan/vulkan.h>




namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	PostRender::PostRender() :
		m_postProcessingCallCount(0)
	{

	}
	PostRender::~PostRender()
	{
		ResetComputeCalls();
	}



	// Movable:
	PostRender::PostRender(PostRender&& other) noexcept = default;
	PostRender& PostRender::operator=(PostRender&& other) noexcept = default;



	// Workload recording:
	emberBackendInterface::IDescriptorSetBinding* PostRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader)
	{
		return RecordComputeShader(pIComputeShader, false);
	}
	emberBackendInterface::IDescriptorSetBinding* PostRender::RecordPostProcessingShader(emberBackendInterface::IComputeShader* pIComputeShader)
	{
		return RecordComputeShader(pIComputeShader, true);
	}



	// Management:
	std::vector<ComputeCall>& PostRender::GetComputeCalls()
	{
		return m_computeCalls;
	}
	size_t PostRender::GetPostProcessingCallCount() const
	{
		return m_postProcessingCallCount;
	}
	void PostRender::ResetComputeCalls()
	{
		// Return all bindings back to the corresponding pool:
		for (ComputeCall& computeCall : m_computeCalls)
		{
			if (computeCall.pComputeShader && computeCall.pCallDescriptorSetBinding)
				PoolManager::ReturnCallDescriptorSetBinding(computeCall.pComputeShader, computeCall.pCallDescriptorSetBinding);
		}

		// Remove all computeCalls so next frame can start fresh:
		m_computeCalls.clear();
		m_postProcessingCallCount = 0;
	}



	// Private methods:
	emberBackendInterface::IDescriptorSetBinding* PostRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, bool isPostProcessing)
	{
		// Record dynamic compute call.
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}

		// Setup compute call:
		uint32_t width = RenderPassManager::GetForwardRenderPass()->GetRenderTexture(0)->GetWidth();
		uint32_t height = RenderPassManager::GetForwardRenderPass()->GetRenderTexture(0)->GetHeight();
		Uint3 threadCount{ width, height, 1 };
		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		DescriptorSetBinding* pDescriptorSetBinding = PoolManager::CheckOutCallDescriptorSetBinding(static_cast<Shader*>(pComputeShader));
		if (!pDescriptorSetBinding)
			return nullptr;
		if (isPostProcessing && (!pDescriptorSetBinding->HasBinding("inputImage") || !pDescriptorSetBinding->HasBinding("outputImage")))
		{
			PoolManager::ReturnCallDescriptorSetBinding(pComputeShader, pDescriptorSetBinding);
			LOG_ERROR("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '{}' must declare CALL_SET storage images named 'inputImage' and 'outputImage'.", pComputeShader->GetName());
			return nullptr;
		}

		ComputeCall computeCall = { threadCount, pComputeShader, pDescriptorSetBinding, AccessMasks::None::none, AccessMasks::None::none, isPostProcessing };
		m_computeCalls.push_back(computeCall);
		if (isPostProcessing)
			m_postProcessingCallCount++;
		return pDescriptorSetBinding;
	}
}