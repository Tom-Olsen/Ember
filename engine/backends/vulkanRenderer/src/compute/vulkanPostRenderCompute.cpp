#include "vulkanPostRenderCompute.h"
#include "descriptorReflection.h"
#include "descriptorSetMacros.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanConvertComputeAccessMask.h"
#include "vulkanDeferredRenderingContract.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanDescriptorTypeToString.h"
#include "vulkanFormatToString.h"
#include "vulkanImageViewTypeToString.h"
#include "vulkanPoolManager.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSceneColorTexture2dPair.h"
#include <stdexcept>
#include <vulkan/vulkan.h>




namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	PostRender::PostRender()
		: m_computeCallQueue(Context::GetFramesInFlight())
	{
		
	}
	PostRender::~PostRender()
	{
		if (!Context::IsDeviceIdle())
			Context::WaitDeviceIdle();
		ResetComputeCalls();
		RetireAllComputeCalls();
	}



	// Movable:
	PostRender::PostRender(PostRender&& other) noexcept = default;
	PostRender& PostRender::operator=(PostRender&& other) noexcept = default;



	// Workload recording:
	emberBackendInterface::IDescriptorSetBinding* PostRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount)
	{
		return RecordComputeShader(pIComputeShader, threadCount, PostProcessingMode::none);
	}
	emberBackendInterface::IDescriptorSetBinding* PostRender::RecordPostProcessingShader(emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount)
	{
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::PostRender::RecordPostProcessingShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}

		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		PostProcessingMode postProcessingMode = DeterminePostProcessingMode(*pComputeShader);
		return RecordComputeShader(pIComputeShader, threadCount, postProcessingMode);
	}

	void PostRender::RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags)
	{
		ComputeCall computeCall = { Uint3::zero, ShaderHandle(), DescriptorSetBindingHandle(), ComputeBarrierFlagsToVulkanAccessMask(srcBarrierFlags), ComputeBarrierFlagsToVulkanAccessMask(dstBarrierFlags) };
		m_computeCallQueue.Add(computeCall);
	}



	// Management:
	void PostRender::CommitComputeCalls(uint32_t frameIndex)
	{
		m_computeCallQueue.Commit(frameIndex);
	}
	void PostRender::RetireAllComputeCalls()
	{
		m_computeCallQueue.RetireAll();
	}
	void PostRender::RetireComputeCalls(uint32_t frameIndex)
	{
		m_computeCallQueue.Retire(frameIndex);
	}
	std::vector<ComputeCall>& PostRender::GetComputeCalls()
	{
		return m_computeCallQueue.GetPendingCalls();
	}
	void PostRender::ResetComputeCalls()
	{
		m_computeCallQueue.DiscardPending();
	}
	void PostRender::UpdateShaderData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair)
	{
		for (ComputeCall& computeCall : m_computeCallQueue.GetPendingCalls())
		{
			if (computeCall.IsBarrier())
				continue;
			if (computeCall.useRenderTextureSize)
				computeCall.threadCount = { sceneColorTexturePair.GetWidth(), sceneColorTexturePair.GetHeight(), 1 };
			switch (computeCall.postProcessingMode)
			{
			case PostProcessingMode::none:
				break;
			case PostProcessingMode::inPlace:
				computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("inOutImage", sceneColorTexturePair.GetCurrentTexture(frameIndex));
				break;
			case PostProcessingMode::outOfPlace:
				computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("inputImage", sceneColorTexturePair.GetCurrentTexture(frameIndex));
				computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("outputImage", sceneColorTexturePair.GetNextTexture(frameIndex));
				sceneColorTexturePair.Swap(frameIndex);
				break;
			}
		}
		m_computeCallQueue.UpdateShaderData(frameIndex);
	}



	// Private methods:
	emberBackendInterface::IDescriptorSetBinding* PostRender::RecordComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount, PostProcessingMode postProcessingMode)
	{
		// Record dynamic compute call.
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}

		bool useRenderTextureSize = postProcessingMode != PostProcessingMode::none && threadCount[0] == 0 && threadCount[1] == 0 && threadCount[2] == 0;
		if (!useRenderTextureSize && (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0))
		{
			LOG_ERROR("compute::PostRender::RecordComputeShader(...) failed. threadCount has 0 entry.");
			return nullptr;
		}

		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(static_cast<Shader*>(pComputeShader));
		DescriptorSetBinding* pDescriptorSetBinding = descriptorSetBindingHandle.Get();
		if (!pDescriptorSetBinding)
			return nullptr;

		// We do not update threadCount=sceneColorTexture.size() here as the size might have changed until the compute call gets dispatched. So its updated in PostRender::UpdateShaderData(...).
		ComputeCall computeCall = { threadCount, ShaderHandle(*pComputeShader), descriptorSetBindingHandle, AccessMasks::None::none, AccessMasks::None::none, postProcessingMode, useRenderTextureSize };
		m_computeCallQueue.Add(computeCall);
		pComputeShader->AddPendingUse();
		return pDescriptorSetBinding;
	}
	PostProcessingMode PostRender::DeterminePostProcessingMode(const ComputeShader& computeShader) const
	{
		const emberSpirvReflect::ShaderReflection& shaderReflection = computeShader.GetShaderReflection();
		const emberSpirvReflect::DescriptorReflection* pInOutImage = shaderReflection.GetDescriptorReflection(CALL_SET_INDEX, "inOutImage");
		const emberSpirvReflect::DescriptorReflection* pInputImage = shaderReflection.GetDescriptorReflection(CALL_SET_INDEX, "inputImage");
		const emberSpirvReflect::DescriptorReflection* pOutputImage = shaderReflection.GetDescriptorReflection(CALL_SET_INDEX, "outputImage");

		// Invalid image bindings:
		if (pInOutImage && (pInputImage || pOutputImage))
			throw std::runtime_error("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' has an ambiguous post-processing interface. CALL_SET must contain either 'inOutImage', or both 'inputImage' and 'outputImage'.");
		if ((pInputImage == nullptr) != (pOutputImage == nullptr))
			throw std::runtime_error("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' has an incomplete out-of-place post-processing interface. CALL_SET must contain both 'inputImage' and 'outputImage'.");

		// In place:
		if (pInOutImage)
		{
			ValidatePostProcessingImage(computeShader, *pInOutImage, false, true, true);
			return PostProcessingMode::inPlace;
		}
		// Out of place:
		if (pInputImage && pOutputImage)
		{
			ValidatePostProcessingImage(computeShader, *pInputImage, true, true, false);
			ValidatePostProcessingImage(computeShader, *pOutputImage, false, false, true);
			return PostProcessingMode::outOfPlace;
		}
		// Invalid:
		throw std::runtime_error("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' has no post-processing interface. CALL_SET must contain either 'inOutImage', or both 'inputImage' and 'outputImage'.");
	}
	void PostRender::ValidatePostProcessingImage(const ComputeShader& computeShader, const emberSpirvReflect::DescriptorReflection& descriptorReflection, bool allowSampledImage, bool requireReadable, bool requireWritable) const
	{
		VkDescriptorType descriptorType = static_cast<VkDescriptorType>(descriptorReflection.GetDescriptorType());
		bool isSupportedDescriptorType = descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE || (allowSampledImage && descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		if (!isSupportedDescriptorType)
		{
			std::string expectedType = allowSampledImage ? "sampled or storage image" : "storage image";
			throw std::runtime_error("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must be a " + expectedType + ", but is " + emberVulkanUtility::ToString(descriptorType) + ".");
		}
		if (descriptorReflection.GetDescriptorCount() != 1)
			throw std::runtime_error("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must have descriptor count 1, but has " + std::to_string(descriptorReflection.GetDescriptorCount()) + ".");

		const emberSpirvReflect::ImageDescriptor* pImageDescriptor = descriptorReflection.GetImageDescriptor();
		if (!pImageDescriptor)
			throw std::runtime_error("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' has no reflected image metadata.");
		if (pImageDescriptor->imageViewType != VK_IMAGE_VIEW_TYPE_2D)
			throw std::runtime_error("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must be a 2D image, but is " + emberVulkanUtility::ToString(pImageDescriptor->imageViewType) + ".");
		if (requireReadable && !pImageDescriptor->isReadable)
			throw std::runtime_error("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must be readable.");
		if (requireWritable && !pImageDescriptor->isWritable)
			throw std::runtime_error("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must be writable.");
		if (descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE && pImageDescriptor->imageFormat != deferredRenderingContract::sceneColorFormat)
			throw std::runtime_error("compute::PostRender::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must use scene color format " + emberVulkanUtility::ToString(deferredRenderingContract::sceneColorFormat) + ", but uses " + emberVulkanUtility::ToString(pImageDescriptor->imageFormat) + ".");
	}
}