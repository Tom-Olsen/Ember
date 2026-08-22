#pragma once
#include <cstdint>
#include <vector>



namespace emberDataStructures
{
	/// <summary>
	/// Allocates stable uint32_t indices and recycles freed indices in constant time.
	/// Free entries form an intrusive singly linked list inside m_entries.
	/// </summary>
	class IndexAllocator
	{
	private: // Types:
		struct Entry
		{
			uint32_t nextFreeIndex;
			bool isAllocated;
		};

	private: // Members:
		std::vector<Entry> m_entries;
		uint32_t m_freeListHead;

	public: // Methods:
		// Constructor/Destructor:
		IndexAllocator();
		~IndexAllocator();

		// Allocate/Free:
		uint32_t Allocate();
		void Free(uint32_t index);

		// Getters:
		bool IsAllocated(uint32_t index) const;
	};
}