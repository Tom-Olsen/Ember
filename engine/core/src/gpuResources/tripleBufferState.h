#pragma once
#include "emberCoreExport.h"
#include <cstdint>



namespace emberEngine
{
	class EMBER_CORE_API TripleBufferState
	{
	public: // Constants:
		static constexpr uint32_t bufferCount = 3;

	private: // Members:
		uint32_t m_srcIndex;
		uint32_t m_dstIndex;
		uint32_t m_readIndex;
		uint32_t m_freeIndex;

	public: // Methods:
		// Constructor/Destructor:
		TripleBufferState();
		~TripleBufferState();

		// State transitions:
		void Reset();
		void CompleteWrite();
		void Publish();

		// Getters:
		uint32_t GetSrcIndex() const;
		uint32_t GetDstIndex() const;
		uint32_t GetReadIndex() const;

    private: // Methods:
		bool HasUnpublishedWrite() const;
	};
}