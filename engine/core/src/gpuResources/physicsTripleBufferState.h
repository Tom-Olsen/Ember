#pragma once
#include "emberCoreExport.h"
#include <array>
#include <cstdint>



namespace emberEngine
{
	// Owns physics buffer index transitions and publishes completed physics writes in submission order.
	class EMBER_CORE_API PhysicsTripleBufferState
	{
	public: // Constants:
		static constexpr uint32_t bufferCount = 3;
		static constexpr uint32_t invalidIndex = bufferCount;

	private: // Subclasses:
		struct PendingWrite
		{
			uint64_t physicsSessionID;
			uint32_t dataIndex;
		};

	private: // Members:
		uint32_t m_srcIndex;
		uint32_t m_dstIndex;
		uint32_t m_readIndex;
		uint32_t m_freeIndex;
		std::array<PendingWrite, bufferCount - 1> m_pendingWrites;
		uint32_t m_pendingWriteCount;

	public: // Methods:
		// Constructor/Destructor:
		PhysicsTripleBufferState();
		~PhysicsTripleBufferState();

		// State transitions:
		void Reset();
		void CommitWrite(uint64_t physicsSessionID);    // commits dst as the new src. It becomes readable after physicsSessionID finishes.
		void PublishFinishedWrites();                   // publishes committed writes whose work has finished, oldest first.

		// Getters:
		uint32_t GetSrcIndex() const;
		uint32_t GetDstIndex() const;
		uint32_t GetReadIndex() const;

	private: // Methods:
        void DiscardPendingWrite(uint32_t dataIndex);
		void AdvanceWriteIndices();
		void RemoveFirstPendingWrite();
		void SetReadIndex(uint32_t dataIndex);
	};
}