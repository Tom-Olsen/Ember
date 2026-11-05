#include "vulkanComputeQueue.h"
#include "logger.h"
#include "vmaBuffer.h"
#include "vulkanAccessMask.h"
#include "vulkanBuffer.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeShader.h"
#include "vulkanConvertComputeAccessMask.h"
#include "vulkanConvertPipelineStage.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanPoolManager.h"
#include "vulkanSceneColorPair.h"
#include <stdexcept>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	ComputeQueue::ComputeQueue(bool supportsSceneColor)
		: m_supportsSceneColor(supportsSceneColor), m_isQueuing(true)
	{
		
	}
	ComputeQueue::~ComputeQueue()
	{
		Reset();
	}



	// Movable:
	ComputeQueue::ComputeQueue(ComputeQueue&& other) noexcept
		: m_supportsSceneColor(other.m_supportsSceneColor), m_isQueuing(other.m_isQueuing)
	{
		m_computeCalls.swap(other.m_computeCalls);
	}
	ComputeQueue& ComputeQueue::operator=(ComputeQueue&& other) noexcept
	{
		if (this != &other)
		{
			Reset();
			m_computeCalls.swap(other.m_computeCalls);
			m_supportsSceneColor = other.m_supportsSceneColor;
			m_isQueuing = other.m_isQueuing;
		}
		return *this;
	}



	// Workload dispatch:
	emberBackendInterface::IDescriptorSetBinding* ComputeQueue::DispatchComputeShader(emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount)
	{
		ValidateQueuing();
		if (!pIComputeShader)
		{
			LOG_ERROR("ComputeQueue::DispatchComputeShader(...) requires a compute shader.");
			return nullptr;
		}
		if (threadCount.x == 0 || threadCount.y == 0 || threadCount.z == 0)
		{
			LOG_ERROR("ComputeQueue::DispatchComputeShader(...) threadCount has a zero dimension.");
			return nullptr;
		}

		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);

		// Compute shader requiring scene color issued to queue that does'nt support it:
		if (!m_supportsSceneColor && (pComputeShader->GetFeatures().ReadsSceneColor() || pComputeShader->GetFeatures().WritesSceneColor()))
		{
			LOG_ERROR("ComputeQueue::DispatchComputeShader(...) '{}' requires scene color, which this queue does not provide.", pComputeShader->GetDebugName());
			return nullptr;
		}

		// Check out call descriptor set binding:
		DescriptorSetBindingHandle bindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(pComputeShader);
		DescriptorSetBinding* pBinding = bindingHandle.Get();
		if (!pBinding)
			return nullptr;

		// Dispatch compute shader call:
		ComputeCall computeCall = { threadCount, ShaderHandle(*pComputeShader), bindingHandle, AccessMasks::None::none, AccessMasks::None::none, pComputeShader->GetFeatures().FlipsSceneColor() };
		try
		{
			m_computeCalls.push_back(computeCall);
		}
		catch (...)
		{
			PoolManager::ReturnCallDescriptorSetBinding(bindingHandle);
			throw;
		}
		pComputeShader->AddPendingUse();
		return pBinding;
	}
	void ComputeQueue::DispatchBufferCopy(emberBackendInterface::IBuffer* pISource, emberBackendInterface::IBuffer* pIDestination, uint64_t size, uint64_t sourceOffset, uint64_t destinationOffset)
	{
		ValidateQueuing();
		if (!pISource || !pIDestination)
			throw std::invalid_argument("ComputeQueue::DispatchBufferCopy(...) requires source and destination buffers.");
		Buffer* pSource = static_cast<Buffer*>(pISource);
		Buffer* pDestination = static_cast<Buffer*>(pIDestination);
		if (!pSource->GetVmaBuffer() || !pDestination->GetVmaBuffer())
			throw std::invalid_argument("ComputeQueue::DispatchBufferCopy(...) buffers must have valid allocations.");
		if (size == 0 || sourceOffset > pISource->GetSize() || size > pISource->GetSize() - sourceOffset || destinationOffset > pIDestination->GetSize() || size > pIDestination->GetSize() - destinationOffset)
			throw std::out_of_range("ComputeQueue::DispatchBufferCopy(...) copy range is empty or outside the buffers.");
		if (pISource == pIDestination && sourceOffset < destinationOffset + size && destinationOffset < sourceOffset + size)
			throw std::invalid_argument("ComputeQueue::DispatchBufferCopy(...) source and destination ranges overlap.");
		if ((pSource->GetVmaBuffer()->GetBufferCreateInfo().usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) == 0 || (pDestination->GetVmaBuffer()->GetBufferCreateInfo().usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) == 0)
			throw std::invalid_argument("ComputeQueue::DispatchBufferCopy(...) buffers do not support the requested transfer usage.");

		ComputeCall computeCall{};
		computeCall.bufferCommand = ComputeCall::BufferCopy{ BufferHandle(*pSource), BufferHandle(*pDestination), size, sourceOffset, destinationOffset };
		m_computeCalls.push_back(computeCall);
	}
	void ComputeQueue::DispatchBufferBarrier(emberBackendInterface::IBuffer* pIBuffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset, uint64_t size)
	{
		ValidateQueuing();
		if (!pIBuffer)
			throw std::invalid_argument("ComputeQueue::DispatchBufferBarrier(...) requires a buffer.");
		Buffer* pBuffer = static_cast<Buffer*>(pIBuffer);
		if (!pBuffer->GetVmaBuffer())
			throw std::invalid_argument("ComputeQueue::DispatchBufferBarrier(...) buffer must have a valid allocation.");
		if (offset >= pIBuffer->GetSize())
			throw std::out_of_range("ComputeQueue::DispatchBufferBarrier(...) offset is outside the buffer.");
		if (size == UINT64_MAX)
			size = pIBuffer->GetSize() - offset;
		if (size == 0 || size > pIBuffer->GetSize() - offset)
			throw std::out_of_range("ComputeQueue::DispatchBufferBarrier(...) range is empty or outside the buffer.");
		if ((sourceStages == emberCommon::PipelineStage::none && sourceAccess != emberCommon::ComputeBarrierFlag::none) || (destinationStages == emberCommon::PipelineStage::none && destinationAccess != emberCommon::ComputeBarrierFlag::none))
			throw std::invalid_argument("ComputeQueue::DispatchBufferBarrier(...) access flags require pipeline stages.");

		ComputeCall computeCall{};
		computeCall.bufferCommand = ComputeCall::BufferBarrier{ BufferHandle(*pBuffer), PipelineStageCommonToVulkan(sourceStages), ComputeBarrierFlagsToVulkanAccessMask(sourceAccess), PipelineStageCommonToVulkan(destinationStages), ComputeBarrierFlagsToVulkanAccessMask(destinationAccess), offset, size };
		m_computeCalls.push_back(computeCall);
	}
	void ComputeQueue::DispatchBarrier(emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags)
	{
		ValidateQueuing();
		ComputeCall computeCall = { Uint3::zero, ShaderHandle(), DescriptorSetBindingHandle(), ComputeBarrierFlagsToVulkanAccessMask(srcBarrierFlags), ComputeBarrierFlagsToVulkanAccessMask(dstBarrierFlags) };
		m_computeCalls.push_back(computeCall);
	}



	// Management:
	void ComputeQueue::Close()
	{
		m_isQueuing = false;
	}
	void ComputeQueue::Reset()
	{
		for (ComputeCall& computeCall : m_computeCalls)
		{
			if (!computeCall.IsDispatch())
				continue;
			PoolManager::ReturnCallDescriptorSetBinding(computeCall.callDescriptorSetBindingHandle);
			computeCall.GetComputeShader()->RemovePendingUse();
		}
		m_computeCalls.clear();
		m_isQueuing = true;
	}
	void ComputeQueue::UpdateShaderData(uint32_t frameIndex, SceneColorPair* pSceneColorPair)
	{
		for (ComputeCall& computeCall : m_computeCalls)
		{
			if (!computeCall.IsDispatch())
				continue;

			// Error on required but missing scene color texture:
			ComputeShader* pComputeShader = computeCall.GetComputeShader();
			if (pComputeShader->GetFeatures().ReadsSceneColor() || pComputeShader->GetFeatures().WritesSceneColor())
			{
				if (!pSceneColorPair)
					throw std::runtime_error("ComputeQueue::UpdateShaderData(...) failed. Shader '" + pComputeShader->GetDebugName() + "' declares scene-color access, but this compute queue has no scene color textures.");
			}

			// Scene color texture asignment and flipping:
			if (pSceneColorPair)
			{
				computeCall.sceneColorIndex = pSceneColorPair->GetCurrentTextureIndex(frameIndex);
				if (computeCall.flipSceneColors)
					pSceneColorPair->Swap(frameIndex);
			}
			pComputeShader->GetDescriptorSetBinding()->UpdateShaderData(frameIndex);
			computeCall.callDescriptorSetBindingHandle.Get()->UpdateShaderData(frameIndex);
		}
	}



	// Getters:
	const std::vector<ComputeCall>& ComputeQueue::GetComputeCalls() const
	{
		return m_computeCalls;
	}
	bool ComputeQueue::IsEmpty() const
	{
		return m_computeCalls.empty();
	}



	// Private methods:
	void ComputeQueue::ValidateQueuing() const
	{
		if (!m_isQueuing)
			throw std::logic_error("ComputeQueue is closed for queuing until its workload has completed.");
	}
}