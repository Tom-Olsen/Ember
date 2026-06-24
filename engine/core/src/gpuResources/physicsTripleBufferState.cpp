#include "physicsTripleBufferState.h"
#include "compute.h"
#include "renderer.h"
#include <cassert>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	PhysicsTripleBufferState::PhysicsTripleBufferState()
	{
		Reset();
	}
	PhysicsTripleBufferState::~PhysicsTripleBufferState()
	{

	}



	// State transitions:
	void PhysicsTripleBufferState::Reset()
	{
		m_srcIndex = 0;
		m_dstIndex = 1;
		m_readIndex = 0;
		m_freeIndex = 2;
		m_graphicsReadFrameIndices.fill(invalidFrameIndex);
		m_pendingWriteCount = 0;
	}
	void PhysicsTripleBufferState::CommitWrite(uint64_t physicsSessionID)
	{
		// The write to dst replaces any older unpublished data in that buffer.
		DiscardPendingWrite(m_dstIndex);
		AdvanceWriteIndices();

		// Only the final committed buffer from one unit of work needs publication.
		if (m_pendingWriteCount > 0 && m_pendingWrites[m_pendingWriteCount - 1].physicsSessionID == physicsSessionID)
		{
			m_pendingWrites[m_pendingWriteCount - 1].dataIndex = m_srcIndex;
			return;
		}

		m_pendingWrites[m_pendingWriteCount++] = { physicsSessionID, m_srcIndex };
	}
	void PhysicsTripleBufferState::PublishFinishedWrites()
	{
		ReleaseFinishedGraphicsReads();
		// Writes must become readable in submission order, so stop at the first unfinished write.
		while (m_pendingWriteCount > 0 && Compute::Physics::IsFinished(m_pendingWrites[0].physicsSessionID))
		{
			SetReadIndex(m_pendingWrites[0].dataIndex);
			RemoveFirstPendingWrite();
		}
	}
	void PhysicsTripleBufferState::MarkRead()
	{
		ReleaseFinishedGraphicsReads();
		m_graphicsReadFrameIndices[m_readIndex] = Renderer::GetFrameIndex();
		if (m_freeIndex == m_readIndex)
			m_freeIndex = invalidIndex;
	}



	// Getters:
	uint32_t PhysicsTripleBufferState::GetSrcIndex() const
	{
		return m_srcIndex;
	}
	uint32_t PhysicsTripleBufferState::GetDstIndex()
	{
		if (m_dstIndex == invalidIndex)
		{
			WaitForFreeIndex();
			m_dstIndex = m_freeIndex;
			m_freeIndex = invalidIndex;
		}
		return m_dstIndex;
	}
	uint32_t PhysicsTripleBufferState::GetReadIndex() const
	{
		return m_readIndex;
	}



	// Private methods:
	bool PhysicsTripleBufferState::CanUseAsFreeIndex(uint32_t dataIndex) const
	{
		return dataIndex < bufferCount
			&& dataIndex != m_srcIndex
			&& dataIndex != m_dstIndex
			&& dataIndex != m_readIndex
			&& m_graphicsReadFrameIndices[dataIndex] == invalidFrameIndex;
	}
	bool PhysicsTripleBufferState::TrySetFreeIndex(uint32_t dataIndex)
	{
		if (m_freeIndex != invalidIndex || !CanUseAsFreeIndex(dataIndex))
			return false;
		m_freeIndex = dataIndex;
		return true;
	}
	void PhysicsTripleBufferState::DiscardPendingWrite(uint32_t dataIndex)
	{
		for (uint32_t i = 0; i < m_pendingWriteCount; i++)
		{
			if (m_pendingWrites[i].dataIndex != dataIndex)
				continue;
			for (uint32_t j = i + 1; j < m_pendingWriteCount; j++)
				m_pendingWrites[j - 1] = m_pendingWrites[j];
			m_pendingWriteCount--;
			return;
		}
	}
	void PhysicsTripleBufferState::AdvanceWriteIndices()
	{
		// The written dst becomes src. Reuse the old src unless rendering still reads it.
		uint32_t oldSrcIndex = m_srcIndex;
		m_srcIndex = m_dstIndex;
		if (oldSrcIndex == m_readIndex)
		{
			if (m_freeIndex == invalidIndex)
				WaitForFreeIndex();
			m_dstIndex = m_freeIndex;
		}
		else
			m_dstIndex = oldSrcIndex;
		m_freeIndex = invalidIndex;
	}
	void PhysicsTripleBufferState::RemoveFirstPendingWrite()
	{
		for (uint32_t i = 1; i < m_pendingWriteCount; i++)
			m_pendingWrites[i - 1] = m_pendingWrites[i];
		m_pendingWriteCount--;
	}
	void PhysicsTripleBufferState::ReleaseFinishedGraphicsReads()
	{
		for (uint32_t i = 0; i < m_graphicsReadFrameIndices.size(); i++)
		{
			if (m_graphicsReadFrameIndices[i] == invalidFrameIndex)
				continue;
			if (!Renderer::IsFrameFinished(m_graphicsReadFrameIndices[i]))
				continue;
			m_graphicsReadFrameIndices[i] = invalidFrameIndex;
			TrySetFreeIndex(i);
		}
	}
	void PhysicsTripleBufferState::SetReadIndex(uint32_t dataIndex)
	{
		if (dataIndex == m_readIndex)
			return;

		// If the published buffer was reserved as the next destination, the old read buffer becomes the new destination:
		uint32_t oldReadIndex = m_readIndex;
		m_readIndex = dataIndex;
		if (m_dstIndex == m_readIndex)
		{
			m_dstIndex = invalidIndex;
			TrySetFreeIndex(oldReadIndex);
			if (m_freeIndex != invalidIndex)
			{
				m_dstIndex = m_freeIndex;
				m_freeIndex = invalidIndex;
			}
		}
		else
			TrySetFreeIndex(oldReadIndex);
	}
	void PhysicsTripleBufferState::WaitForFreeIndex()
	{
		ReleaseFinishedGraphicsReads();
		if (m_freeIndex != invalidIndex)
			return;

		for (uint32_t i = 0; i < m_graphicsReadFrameIndices.size(); i++)
		{
			if (i == m_srcIndex || i == m_dstIndex || i == m_readIndex)
				continue;
			if (m_graphicsReadFrameIndices[i] != invalidFrameIndex)
			{
				Renderer::WaitForFrameFinished(m_graphicsReadFrameIndices[i]);
				m_graphicsReadFrameIndices[i] = invalidFrameIndex;
			}
			if (TrySetFreeIndex(i))
				return;
		}
		assert(false && "PhysicsTripleBufferState::WaitForFreeIndex failed to find a free buffer.");
	}
}