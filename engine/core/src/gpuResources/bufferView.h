
#pragma once
#include "buffer.h"
#include "emberCoreExport.h"
#include <cstdint>
#include <string>



namespace emberEngine
{
    // Forward decleration:
    class Buffer;



    template<typename T>
    class EMBER_CORE_API BufferView
    {
    private: // Members:
        Buffer* m_pBuffer;
        uint32_t m_offset;
        uint32_t m_count;

    public: // Methods:
		// Constructor/Destructor:
        BufferView();
        BufferView(Buffer& buffer);
        BufferView(Buffer& buffer, uint32_t offset, uint32_t count);
        ~BufferView();

		// Copyable:
		BufferView(const BufferView&) = default;
		BufferView& operator=(const BufferView&) = default;

		// Movable:
		BufferView(BufferView&& other) noexcept;
		BufferView& operator=(BufferView&& other) noexcept;

        // Getters:
		std::string GetName() const;
        uint64_t GetSize() const;
        uint32_t GetCount() const;
		uint32_t GetElementSize() const;
        bool IsValid() const;
        Buffer& GetBuffer() const;
        uint32_t GetOffset() const;

        // Setters:
        void SetBuffer(Buffer& buffer);
        void SetOffset(uint32_t offset);
        void SetCount(uint32_t count);

        // Upload/Download:
        void Upload(const void* pSrc, uint64_t count);
        void Download(void* pDst, uint64_t count);
    };
}