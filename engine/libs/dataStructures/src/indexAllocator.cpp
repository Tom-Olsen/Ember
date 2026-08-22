#include "indexAllocator.h"
#include <limits>
#include <stdexcept>



namespace emberDataStructures
{
	static constexpr uint32_t invalidIndex = std::numeric_limits<uint32_t>::max();


	// Public methods:
	// Constructor/Destructor:
	IndexAllocator::IndexAllocator()
		: m_freeListHead(invalidIndex)
	{

	}
	IndexAllocator::~IndexAllocator()
	{

	}



	// Allocate/Free:
	uint32_t IndexAllocator::Allocate()
	{
		if (m_freeListHead == invalidIndex)
		{
			if (m_entries.size() >= invalidIndex)
				throw std::overflow_error("IndexAllocator::Allocate() failed. No more uint32_t indices are available.");
			uint32_t index = static_cast<uint32_t>(m_entries.size());
			m_entries.push_back(Entry{invalidIndex, true});
			return index;
		}

		uint32_t index = m_freeListHead;
		Entry& entry = m_entries[index];
		m_freeListHead = entry.nextFreeIndex;
		entry.nextFreeIndex = invalidIndex;
		entry.isAllocated = true;
		return index;
	}
	void IndexAllocator::Free(uint32_t index)
	{
		if (index >= m_entries.size())
			throw std::out_of_range("IndexAllocator::Free(...) failed. Index is out of range.");
		if (!m_entries[index].isAllocated)
			throw std::runtime_error("IndexAllocator::Free(...) failed. Index is already free.");

		Entry& entry = m_entries[index];
		entry.isAllocated = false;
		entry.nextFreeIndex = m_freeListHead;
		m_freeListHead = index;
	}



	// Getters:
	bool IndexAllocator::IsAllocated(uint32_t index) const
	{
		return index < m_entries.size() && m_entries[index].isAllocated;
	}
}