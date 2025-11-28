
#pragma once
#include "buffer.h"
#include "bufferView.h"
#include "emberCoreExport.h"
#include <cstdint>
#include <string>



namespace emberEngine
{
    // Forward decleration:
    class Buffer;



    template<typename T>
    class EMBER_CORE_API BufferTyped
    {
    private: // Members:
        Buffer m_buffer;
        BufferView<T> m_bufferView;

    public: // Methods:
        // Constructor/Destructor:
        BufferTyped();
        BufferTyped(uint32_t count, const std::string& name, emberCommon::BufferUsage usage);
        ~BufferTyped();

        // Non-copyable:
        BufferTyped(const BufferTyped&) = delete;
        BufferTyped& operator=(const BufferTyped&) = delete;

        // Movable:
        BufferTyped(BufferTyped&& other) noexcept = default;
        BufferTyped& operator=(BufferTyped&& other) noexcept = default;

        // Getters:
        std::string GetName() const;
        uint64_t GetSize() const;
        uint32_t GetCount() const;
        uint32_t GetElementSize() const;
        bool IsValid() const;
        Buffer& GetBuffer();
        BufferView<T>& GetBufferView();

        // Upload/Download:
        void Upload(const void* pSrc, uint64_t count);
        void Download(void* pDst, uint64_t count);
    };
}