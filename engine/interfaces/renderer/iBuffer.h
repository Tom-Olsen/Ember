#pragma once
#include <cstdint>
#include <string>



namespace emberBackendInterface
{
    class IBuffer
    {
    public:
        // Virtual destructor for v.table:
        virtual ~IBuffer() = default;

        virtual std::string GetName() const = 0;
        virtual uint64_t GetSize() const = 0;
        virtual uint32_t GetCount() const = 0;
        virtual uint32_t GetElementSize() const = 0;

        virtual void Upload(const void* src, uint64_t size) = 0;
        virtual void Download(void* dst, uint64_t size) = 0;
    };
}