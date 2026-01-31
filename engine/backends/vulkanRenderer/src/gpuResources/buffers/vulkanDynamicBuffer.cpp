#include "vulkanDynamicBuffer.h"
#include "vulkanContext.h"
#include "vulkanIndexBuffer.h"
#include "vulkanStorageBuffer.h"
#include "vulkanVertexBuffer.h"



namespace vulkanRendererBackend
{
    // Public methods:
    // Constructor/Destructor:
    template<typename BufferType>
    DynamicBuffer<BufferType>::DynamicBuffer(uint32_t count, uint32_t elementSize, const std::string& name)
    {
        uint32_t framesInFlight = Context::GetFramesInFlight();
        m_buffers.resize(framesInFlight);
        for (uint32_t i = 0; i < framesInFlight; ++i)
            m_buffers[i] = std::make_unique<BufferType>(count, elementSize, name);
    }
    template<typename BufferType>
    DynamicBuffer<BufferType>::~DynamicBuffer()
    {
        m_buffers.clear();
    }

    

    // Getters:
    template<typename BufferType>
    std::string DynamicBuffer<BufferType>::GetName() const
    {
        return m_buffers[0]->GetName();
    }
    template<typename BufferType>
    uint64_t DynamicBuffer<BufferType>::GetSize() const
    {
        return m_buffers[0]->GetSize();
    }
    template<typename BufferType>
    uint32_t DynamicBuffer<BufferType>::GetCount() const
    {
        return m_buffers[0]->GetCount();
    }
    template<typename BufferType>
    uint32_t DynamicBuffer<BufferType>::GetElementSize() const
    {
        return m_buffers[0]->GetElementSize();
    }


    
    // Data transfer:
    template<typename BufferType>
    void DynamicBuffer<BufferType>::Upload(const void* pSrc, uint64_t size)
    {
        // Ember::ToDo: implement upload pipeline for multi frame buffers
    }
    template<typename BufferType>
    void DynamicBuffer<BufferType>::Download(void* pDst, uint64_t size)
    {
        // Ember::ToDo: implement download pipeline for multi frame buffers
    }



    // Backend only:
    template<typename BufferType>
    BufferType& DynamicBuffer<BufferType>::GetBuffer()
    {
        return *m_buffers[Context::GetCurrentFrameIndex()];
    }
    template<typename BufferType>
    BufferType& DynamicBuffer<BufferType>::GetBuffer(uint32_t frameIndex)
    {
        return *m_buffers[frameIndex];
    }
    template<typename BufferType>
    void DynamicBuffer<BufferType>::Upload(VkCommandBuffer vkCommandBuffer, void* pSrc, uint64_t size)
    {
        // Ember::ToDo: implement upload pipeline for multi frame buffers
    }
    template<typename BufferType>
    void DynamicBuffer<BufferType>::Download(VkCommandBuffer vkCommandBuffer, void* pDst, uint64_t size)
    {
        // Ember::ToDo: implement download pipeline for multi frame buffers
    }
}



// Explicit template instantiations:
template class vulkanRendererBackend::DynamicBuffer<vulkanRendererBackend::IndexBuffer>;
template class vulkanRendererBackend::DynamicBuffer<vulkanRendererBackend::StorageBuffer>;
template class vulkanRendererBackend::DynamicBuffer<vulkanRendererBackend::VertexBuffer>;