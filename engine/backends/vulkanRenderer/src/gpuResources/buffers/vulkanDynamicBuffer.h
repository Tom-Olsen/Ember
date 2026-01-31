#pragma once
#include "iBuffer.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <vector>



namespace vulkanRendererBackend
{
    template<typename BufferType>
    class VULKAN_RENDERER_API DynamicBuffer : public emberBackendInterface::IBuffer
    {
    private:
        std::vector<std::unique_ptr<BufferType>> m_buffers;

    public:
        // Constructor/Destructor:
        DynamicBuffer(uint32_t count, uint32_t elementSize, const std::string& name);
        ~DynamicBuffer();

		// Non-copyable:
		DynamicBuffer(const DynamicBuffer&) = delete;
		DynamicBuffer& operator=(const DynamicBuffer&) = delete;

		// Movable:
		DynamicBuffer(DynamicBuffer&&) noexcept;
		DynamicBuffer& operator=(DynamicBuffer&&) noexcept;

        // Getters:
        std::string GetName() const override;
        uint64_t GetSize() const override;
        uint32_t GetCount() const override;
        uint32_t GetElementSize() const override;

        // Data transfer:
        void Upload(const void* pSrc, uint64_t size) override;
        void Download(void* pDst, uint64_t size) override;

        // Backend only:
        BufferType& GetBuffer();
        BufferType& GetBuffer(uint32_t frameIndex);
		void Upload(VkCommandBuffer vkCommandBuffer, void* pSrc, uint64_t size);
		void Download(VkCommandBuffer vkCommandBuffer, void* pDst, uint64_t size);
    };
}