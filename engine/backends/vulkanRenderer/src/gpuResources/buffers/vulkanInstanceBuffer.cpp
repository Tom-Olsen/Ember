#include "vulkanInstanceBuffer.h"
#include "instanceData.h"
#include "vulkanContext.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	InstanceBuffer::InstanceBuffer(uint32_t count)
		: StorageBuffer(count, sizeof(emberCommon::InstanceData))
		, m_pHistoryBuffer(std::make_unique<StorageBuffer>(count, sizeof(emberCommon::InstanceData)))
		, m_initFrameIndex(UINT64_MAX)
		, m_lastFrameIndex(UINT64_MAX)
	{

	}
	InstanceBuffer::~InstanceBuffer()
	{

	}



	// History:
	bool InstanceBuffer::HasHistory() const
	{
		return m_initFrameIndex < Context::GetAbsoluteFrameIndex();
	}
	emberBackendInterface::IBuffer& InstanceBuffer::GetBufferOld()
	{
		if (HasHistory())
			return *m_pHistoryBuffer;
		return *this;
	}
	void InstanceBuffer::ResetHistory()
	{
		uint64_t absoluteFrameIndex = Context::GetAbsoluteFrameIndex();
		if (m_lastFrameIndex == absoluteFrameIndex)
			m_initFrameIndex = absoluteFrameIndex;	// copy alread queued. Will serve as initialization once executed.
		else
			m_initFrameIndex = UINT64_MAX;			// invalidate initialization.
	}



	// History copy requests:
	void InstanceBuffer::DispatchCopyToOldBuffer(emberBackendInterface::ICompute::IQueue& queue)
	{
		using emberCommon::ComputeBarrierFlag;
		using emberCommon::PipelineStage;

		uint64_t absoluteFrameIndex = Context::GetAbsoluteFrameIndex();
		if (m_lastFrameIndex == absoluteFrameIndex)
			return;
		PipelineStage stages = PipelineStage::vertexShader | PipelineStage::computeShader | PipelineStage::copy;

		// Make current instance data readable by the copy:
		queue.DispatchBufferBarrier(this, stages,
			ComputeBarrierFlag::shaderRead | ComputeBarrierFlag::shaderWrite | ComputeBarrierFlag::transferWrite,
			PipelineStage::copy, ComputeBarrierFlag::transferRead);
		// Finish reading old instance data before overwriting it:
		queue.DispatchBufferBarrier(m_pHistoryBuffer.get(), stages,
			ComputeBarrierFlag::shaderRead | ComputeBarrierFlag::transferRead | ComputeBarrierFlag::transferWrite,
			PipelineStage::copy, ComputeBarrierFlag::transferWrite);

		queue.DispatchBufferCopy(this, m_pHistoryBuffer.get(), m_size);

		// Make the copied data readable by the next frame's vertex shaders:
		queue.DispatchBufferBarrier(m_pHistoryBuffer.get(), PipelineStage::copy, ComputeBarrierFlag::transferWrite,
			PipelineStage::vertexShader, ComputeBarrierFlag::shaderRead);
		// Finish copying current data before subsequent updates can overwrite it:
		queue.DispatchBufferBarrier(this, stages,
			ComputeBarrierFlag::shaderRead | ComputeBarrierFlag::shaderWrite | ComputeBarrierFlag::transferRead | ComputeBarrierFlag::transferWrite,
			stages, ComputeBarrierFlag::transferWrite | ComputeBarrierFlag::shaderRead | ComputeBarrierFlag::shaderWrite);

		// Initialization:
		if (m_initFrameIndex == UINT64_MAX)
			m_initFrameIndex = absoluteFrameIndex;
		m_lastFrameIndex = absoluteFrameIndex;
	}



	// Debugging:
	void InstanceBuffer::SetDebugName(const std::string& name)
	{
		StorageBuffer::SetDebugName(name);
		m_pHistoryBuffer->SetDebugName(name + "_Old");
	}
}