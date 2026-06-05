#pragma once
#include "bufferTyped.h"
#include "emberCoreExport.h"
#include <array>
#include <cstdint>
#include <string>
#include <vector>



namespace emberEngine
{
	template<typename T>
	class EMBER_CORE_API TripleBufferedBuffer
	{
	public: // Constants:
		static constexpr uint32_t bufferCount = 3;

	private: // Members:
		std::array<BufferTyped<T>, bufferCount> m_buffers;

	public: // Methods:
		// Constructor/Destructor:
		TripleBufferedBuffer();
		TripleBufferedBuffer(uint32_t count, const std::string& name, emberCommon::BufferUsage usage);
		~TripleBufferedBuffer();

		// Non-copyable:
		TripleBufferedBuffer(const TripleBufferedBuffer&) = delete;
		TripleBufferedBuffer& operator=(const TripleBufferedBuffer&) = delete;

		// Movable:
		TripleBufferedBuffer(TripleBufferedBuffer&& other) noexcept;
		TripleBufferedBuffer& operator=(TripleBufferedBuffer&& other) noexcept;

		// Getters:
		std::string GetName(uint32_t bufferIndex = 0) const;
		uint64_t GetSize(uint32_t bufferIndex = 0) const;
		uint32_t GetCount(uint32_t bufferIndex = 0) const;
		uint32_t GetElementSize() const;
		bool IsValid() const;
		Buffer& GetBuffer(uint32_t bufferIndex);
		BufferView<T>& GetBufferView(uint32_t bufferIndex);

		// Upload/Download:
		void Upload(uint32_t bufferIndex, const void* pSrc, uint64_t count);
		void Upload(uint32_t bufferIndex, const std::vector<T>& src);
		void Download(uint32_t bufferIndex, void* pDst, uint64_t count);
		void Download(uint32_t bufferIndex, std::vector<T>& dst);
	};
}