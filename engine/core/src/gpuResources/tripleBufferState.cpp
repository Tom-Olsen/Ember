#include "tripleBufferState.h"



namespace emberEngine
{
	// Public methods:
	// Constructor/Destructor:
	TripleBufferState::TripleBufferState()
	{
		Reset();
	}
	TripleBufferState::~TripleBufferState()
	{

	}



	// State transitions:
	void TripleBufferState::Reset()
	{
		m_srcIndex = 0;
		m_dstIndex = 1;
		m_readIndex = 0;
		m_freeIndex = 2;
	}
	void TripleBufferState::CompleteWrite()
	{
		uint32_t oldSrcIndex = m_srcIndex;
		m_srcIndex = m_dstIndex;
		m_dstIndex = (oldSrcIndex == m_readIndex) ? m_freeIndex : oldSrcIndex;
		m_freeIndex = bufferCount; // invalid
	}
	void TripleBufferState::Publish()
	{
		if (m_srcIndex == m_readIndex)
			return;
		uint32_t oldReadIndex = m_readIndex;
		m_readIndex = m_srcIndex;
		m_freeIndex = oldReadIndex;
	}



	// Getters:
	uint32_t TripleBufferState::GetSrcIndex() const
	{
		return m_srcIndex;
	}
	uint32_t TripleBufferState::GetDstIndex() const
	{
		return m_dstIndex;
	}
	uint32_t TripleBufferState::GetReadIndex() const
	{
		return m_readIndex;
	}
	bool TripleBufferState::HasUnpublishedWrite() const
	{
		return m_srcIndex != m_readIndex;
	}
}