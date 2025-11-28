#include "bufferView.h"
#include "emberMath.h"
#include "logger.h"



namespace emberEngine
{
    // Public methods:
	// Constructor/Destructor:
    template<typename T>
    BufferView<T>::BufferView()
    {
        m_pBuffer = nullptr;
        m_offset = 0;
        m_count = 0;
    }
    template<typename T>
    BufferView<T>::BufferView(Buffer& buffer)
    {
        m_pBuffer = &buffer;
        m_offset = 0;
        m_count = buffer.GetCount();
    }
    template<typename T>
    BufferView<T>::BufferView(Buffer& buffer, uint32_t offset, uint32_t count)
    {
        m_pBuffer = &buffer;
        m_offset = offset;

        uint32_t maxCount = buffer.GetCount() - offset;
        if (count > maxCount)
        {
            LOG_WARN("Trying to create BufferView<T> with out of bounds elements: name={}, offset={}, count={}, maxCount={}", GetName(), offset, count, maxCount);
            m_count = maxCount;
        }
        else
            m_count = count;
    }



    // Getters:
    template<typename T>
    std::string BufferView<T>::GetName() const
    {
        return m_pBuffer->GetName();
    }
    template<typename T>
    uint64_t BufferView<T>::GetSize() const
    {
        return static_cast<uint64_t>(m_count) * sizeof(T);
    }
    template<typename T>
    uint32_t BufferView<T>::GetCount() const
    {
        return m_count;
    }
    template<typename T>
    uint32_t BufferView<T>::GetElementSize() const
    {
        return sizeof(T);
    }
    template<typename T>
    bool BufferView<T>::IsValid() const
    {
        return m_pBuffer && m_pBuffer->IsValid() && (m_offset + m_count) <= m_pBuffer->GetCount();
    }
    template<typename T>
    Buffer& BufferView<T>::GetBuffer() const
    {
        return *m_pBuffer;
    }
    template<typename T>
    uint32_t BufferView<T>::GetOffset() const
    {
        return m_offset;
    }



    // Setters:
    template<typename T>
    void BufferView<T>::SetOffset(uint32_t offset)
    {
        m_offset = offset;
    }
    template<typename T>
    void BufferView<T>::SetCount(uint32_t count)
    {
        m_count = count;
    }



    // Upload/Download:
    template<typename T>
    void BufferView<T>::Upload(const void* pSrc, uint64_t count)
    {
        m_pBuffer->Upload(pSrc, count * sizeof(T));
    }
    template<typename T>
    void BufferView<T>::Download(void* pDst, uint64_t count)
    {
        m_pBuffer->Download(pDst, count * sizeof(T));
    }


    
    // Explicit template instantiations:
    template class BufferView<int>;
    template class BufferView<Int2>;
    template class BufferView<Int3>;
    template class BufferView<uint32_t>;
    template class BufferView<Uint2>;
    template class BufferView<Uint3>;
    template class BufferView<float>;
    template class BufferView<Float2>;
    template class BufferView<Float3>;
    template class BufferView<Float4>;
    template class BufferView<Float2x2>;
    template class BufferView<Float2x3>;
    template class BufferView<Float3x2>;
    template class BufferView<Float3x3>;
    template class BufferView<Float4x4>;
}