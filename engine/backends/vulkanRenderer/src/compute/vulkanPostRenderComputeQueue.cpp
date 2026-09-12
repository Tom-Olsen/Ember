#include "vulkanPostRenderComputeQueue.h"
#include "descriptorReflection.h"
#include "descriptorSetMacros.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanDeferredRenderingContract.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanDescriptorTypeToString.h"
#include "vulkanFormatToString.h"
#include "vulkanImageViewTypeToString.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSceneColorTexture2dPair.h"
#include <stdexcept>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	PostRenderComputeQueue::PostRenderComputeQueue() = default;
	PostRenderComputeQueue::~PostRenderComputeQueue() = default;



	// Movable:
	PostRenderComputeQueue::PostRenderComputeQueue(PostRenderComputeQueue&& other) noexcept = default;
	PostRenderComputeQueue& PostRenderComputeQueue::operator=(PostRenderComputeQueue&& other) noexcept = default;



	// Workload recording:
	emberBackendInterface::IDescriptorSetBinding* PostRenderComputeQueue::RecordPostProcessingShader(emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount)
	{
		if (!pIComputeShader)
		{
			LOG_ERROR("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}

		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		SceneColorBindingMode sceneColorBindingMode = DetermineSceneColorBindingMode(*pComputeShader);
		bool useRenderTextureSize = threadCount[0] == 0 && threadCount[1] == 0 && threadCount[2] == 0;
		if (!useRenderTextureSize && (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0))
		{
			LOG_ERROR("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. threadCount has 0 entry.");
			return nullptr;
		}
		ComputeCall computeCall = { threadCount, ShaderHandle(*pComputeShader), DescriptorSetBindingHandle(), AccessMasks::None::none, AccessMasks::None::none, sceneColorBindingMode, useRenderTextureSize };
		return RecordComputeCall(computeCall);
	}



	// Management:
	void PostRenderComputeQueue::UpdateShaderData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair)
	{
		for (ComputeCall& computeCall : GetComputeCalls())
		{
			if (computeCall.IsBarrier())
				continue;
			if (computeCall.useRenderTextureSize)
				computeCall.threadCount = { sceneColorTexturePair.GetWidth(), sceneColorTexturePair.GetHeight(), 1 };

			switch (computeCall.sceneColorBindingMode)
			{
			case SceneColorBindingMode::none:
				break;
			case SceneColorBindingMode::inPlace:
				computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("inOutImage", sceneColorTexturePair.GetCurrentTexture(frameIndex), VK_IMAGE_LAYOUT_GENERAL);
				break;
			case SceneColorBindingMode::outOfPlace:
				computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("inputImage", sceneColorTexturePair.GetCurrentTexture(frameIndex), VK_IMAGE_LAYOUT_GENERAL);
				computeCall.callDescriptorSetBindingHandle.Get()->SetTexture("outputImage", sceneColorTexturePair.GetNextTexture(frameIndex), VK_IMAGE_LAYOUT_GENERAL);
				sceneColorTexturePair.Swap(frameIndex);
				break;
			}
		}
		ComputeQueue::UpdateShaderData(frameIndex);
	}



	// Private methods:
	SceneColorBindingMode PostRenderComputeQueue::DetermineSceneColorBindingMode(const ComputeShader& computeShader) const
	{
		const emberSpirvReflect::ShaderReflection& shaderReflection = computeShader.GetShaderReflection();
		const emberSpirvReflect::DescriptorReflection* pInOutImage = shaderReflection.GetDescriptorReflection(CALL_SET_INDEX, "inOutImage");
		const emberSpirvReflect::DescriptorReflection* pInputImage = shaderReflection.GetDescriptorReflection(CALL_SET_INDEX, "inputImage");
		const emberSpirvReflect::DescriptorReflection* pOutputImage = shaderReflection.GetDescriptorReflection(CALL_SET_INDEX, "outputImage");

		// Invalid image bindings:
		if (pInOutImage && (pInputImage || pOutputImage))
			throw std::runtime_error("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' has an ambiguous scene-color binding contract. CALL_SET must contain either 'inOutImage', or both 'inputImage' and 'outputImage'.");
		if ((pInputImage == nullptr) != (pOutputImage == nullptr))
			throw std::runtime_error("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' has an incomplete out-of-place scene-color binding contract. CALL_SET must contain both 'inputImage' and 'outputImage'.");

		// In place:
		if (pInOutImage)
		{
			ValidateSceneColorImage(computeShader, *pInOutImage, false, true, true);
			return SceneColorBindingMode::inPlace;
		}
		// Out of place:
		if (pInputImage && pOutputImage)
		{
			ValidateSceneColorImage(computeShader, *pInputImage, true, true, false);
			ValidateSceneColorImage(computeShader, *pOutputImage, false, false, true);
			return SceneColorBindingMode::outOfPlace;
		}
		// Invalid:
		throw std::runtime_error("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' has no scene-color binding contract. CALL_SET must contain either 'inOutImage', or both 'inputImage' and 'outputImage'.");
	}
	void PostRenderComputeQueue::ValidateSceneColorImage(const ComputeShader& computeShader, const emberSpirvReflect::DescriptorReflection& descriptorReflection, bool allowSampledImage, bool requireReadable, bool requireWritable) const
	{
		VkDescriptorType descriptorType = static_cast<VkDescriptorType>(descriptorReflection.GetDescriptorType());
		bool isSupportedDescriptorType = descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE || (allowSampledImage && descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		if (!isSupportedDescriptorType)
		{
			std::string expectedType = allowSampledImage ? "sampled or storage image" : "storage image";
			throw std::runtime_error("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must be a " + expectedType + ", but is " + emberVulkanUtility::ToString(descriptorType) + ".");
		}
		if (descriptorReflection.GetDescriptorCount() != 1)
			throw std::runtime_error("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must have descriptor count 1, but has " + std::to_string(descriptorReflection.GetDescriptorCount()) + ".");

		const emberSpirvReflect::ImageDescriptor* pImageDescriptor = descriptorReflection.GetImageDescriptor();
		if (!pImageDescriptor)
			throw std::runtime_error("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' has no reflected image metadata.");
		if (pImageDescriptor->imageViewType != VK_IMAGE_VIEW_TYPE_2D)
			throw std::runtime_error("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must be a 2D image, but is " + emberVulkanUtility::ToString(pImageDescriptor->imageViewType) + ".");
		if (requireReadable && !pImageDescriptor->isReadable)
			throw std::runtime_error("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must be readable.");
		if (requireWritable && !pImageDescriptor->isWritable)
			throw std::runtime_error("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must be writable.");
		if (descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE && pImageDescriptor->imageFormat != deferredRenderingContract::sceneColorFormat)
			throw std::runtime_error("PostRenderComputeQueue::RecordPostProcessingShader(...) failed. Compute shader '" + computeShader.GetDebugName() + "' CALL_SET binding '" + descriptorReflection.GetName() + "' must use scene color format " + emberVulkanUtility::ToString(deferredRenderingContract::sceneColorFormat) + ", but uses " + emberVulkanUtility::ToString(pImageDescriptor->imageFormat) + ".");
	}
}