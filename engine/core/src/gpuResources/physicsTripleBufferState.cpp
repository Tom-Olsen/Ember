#include "physicsTripleBufferState.h"
#include "compute.h"
#include <cassert>



namespace emberEngine
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
		// Writes must become readable in submission order, so stop at the first unfinished write.
		while (m_pendingWriteCount > 0 && Compute::Physics::IsFinished(m_pendingWrites[0].physicsSessionID))
		{
			SetReadIndex(m_pendingWrites[0].dataIndex);
			RemoveFirstPendingWrite();
		}
	}



	// Getters:
	uint32_t PhysicsTripleBufferState::GetSrcIndex() const
	{
		return m_srcIndex;
	}
	uint32_t PhysicsTripleBufferState::GetDstIndex() const
	{
		return m_dstIndex;
	}
	uint32_t PhysicsTripleBufferState::GetReadIndex() const
	{
		return m_readIndex;
	}



	// Private methods:
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
		m_dstIndex = (oldSrcIndex == m_readIndex) ? m_freeIndex : oldSrcIndex;
		m_freeIndex = invalidIndex;
	}
	void PhysicsTripleBufferState::RemoveFirstPendingWrite()
	{
		for (uint32_t i = 1; i < m_pendingWriteCount; i++)
			m_pendingWrites[i - 1] = m_pendingWrites[i];
		m_pendingWriteCount--;
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
			m_dstIndex = oldReadIndex;
			m_freeIndex = invalidIndex;
		}
		else
			m_freeIndex = oldReadIndex;
	}
}