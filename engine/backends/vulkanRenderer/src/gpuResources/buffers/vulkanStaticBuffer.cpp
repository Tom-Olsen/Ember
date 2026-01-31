#include "vulkanStaticBuffer.h"
#include "vulkanContext.h"
#include "vulkanIndexBuffer.h"
#include "vulkanStorageBuffer.h"
#include "vulkanVertexBuffer.h"



namespace vulkanRendererBackend
{
    // Public methods:
    // Constructor/Destructor:
    template<typename BufferType>
    StaticBuffer<BufferType>::StaticBuffer(uint32_t count, uint32_t elementSize, const std::string& name)
    {
        m_buffer = std::make_unique<BufferType>(count, elementSize, name);
    }
    template<typename BufferType>
    StaticBuffer<BufferType>::~StaticBuffer()
    {
        m_buffer.reset();
    }

    

    // Getters:
    template<typename BufferType>
    std::string StaticBuffer<BufferType>::GetName() const
    {
        return m_buffers[0]->GetName();
    }
    template<typename BufferType>
    uint64_t StaticBuffer<BufferType>::GetSize() const
    {
        return m_buffers[0]->GetSize();
    }
    template<typename BufferType>
    uint32_t StaticBuffer<BufferType>::GetCount() const
    {
        return m_buffers[0]->GetCount();
    }
    template<typename BufferType>
    uint32_t StaticBuffer<BufferType>::GetElementSize() const
    {
        return m_buffers[0]->GetElementSize();
    }


    
    // Data transfer:
    template<typename BufferType>
    void StaticBuffer<BufferType>::Upload(const void* pSrc, uint64_t size)
    {
        // Ember::ToDo: implement upload pipeline for multi frame buffers
    }
    template<typename BufferType>
    void StaticBuffer<BufferType>::Download(void* pDst, uint64_t size)
    {
        // Ember::ToDo: implement download pipeline for multi frame buffers
    }



    // Backend only:
    template<typename BufferType>
    BufferType& StaticBuffer<BufferType>::GetBuffer()
    {
        return *m_buffer.get();
    }
    template<typename BufferType>
    void StaticBuffer<BufferType>::Upload(VkCommandBuffer vkCommandBuffer, void* pSrc, uint64_t size)
    {
        // Ember::ToDo: implement upload pipeline for multi frame buffers
    }
    template<typename BufferType>
    void StaticBuffer<BufferType>::Download(VkCommandBuffer vkCommandBuffer, void* pDst, uint64_t size)
    {
        // Ember::ToDo: implement download pipeline for multi frame buffers
    }
}



// Explicit template instantiations:
template class vulkanRendererBackend::StaticBuffer<vulkanRendererBackend::IndexBuffer>;
template class vulkanRendererBackend::StaticBuffer<vulkanRendererBackend::StorageBuffer>;
template class vulkanRendererBackend::StaticBuffer<vulkanRendererBackend::VertexBuffer>;