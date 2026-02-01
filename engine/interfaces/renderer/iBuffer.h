#pragma once
#include <cstdint>
#include <string>



namespace emberBackendInterface
{
    class IBuffer
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~IBuffer() = default;

        // Getters:
        virtual uint64_t GetSize() const = 0;
        virtual uint32_t GetCount() const = 0;
        virtual uint32_t GetElementSize() const = 0;

        // Data transfer:
        virtual void Upload(const void* pSrc, uint64_t size) = 0;
        virtual void Download(void* pDst, uint64_t size) = 0;

        // Debugging:
        virtual void SetDebugName(const std::string& name);
    };
}