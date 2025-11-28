#include "bufferTyped.h"
#include "emberMath.h"



namespace emberEngine
{
	// Public methods:
	// Constructor/Destructor:
	template<typename T>
	BufferTyped<T>::BufferTyped()
	{

	}
	template<typename T>
	BufferTyped<T>::BufferTyped(uint32_t count, const std::string& name, emberCommon::BufferUsage usage)
	{
		m_buffer = Buffer(count, sizeof(T), name, usage);
        m_bufferView = BufferView<T>(m_buffer);
	}
	template<typename T>
	BufferTyped<T>::~BufferTyped()
	{

	}



    // Getters:
    template<typename T>
    std::string BufferTyped<T>::GetName() const
    {
        return m_buffer.GetName();
    }
    template<typename T>
    uint64_t BufferTyped<T>::GetSize() const
    {
        return m_buffer.GetSize();
    }
    template<typename T>
    uint32_t BufferTyped<T>::GetCount() const
    {
        return m_buffer.GetCount();
    }
    template<typename T>
    uint32_t BufferTyped<T>::GetElementSize() const
    {
        return sizeof(T);
    }
    template<typename T>
    bool BufferTyped<T>::IsValid() const
    {
        return m_buffer.IsValid();
    }
    template<typename T>
    Buffer& BufferTyped<T>::GetBuffer()
    {
        return m_buffer;
    }
    template<typename T>
    BufferView<T>& BufferTyped<T>::GetBufferView()
    {
        return m_bufferView;
    }



    // Upload/Download:
    template<typename T>
    void BufferTyped<T>::Upload(const void* pSrc, uint64_t count)
    {
        m_buffer.Upload(pSrc, count * sizeof(T));
    }
    template<typename T>
    void BufferTyped<T>::Download(void* pDst, uint64_t count)
    {
        m_buffer.Download(pDst, count * sizeof(T));
    }



    // Explicit template instantiations:
    template class BufferTyped<int>;
    template class BufferTyped<Int2>;
    template class BufferTyped<Int3>;
    template class BufferTyped<uint32_t>;
    template class BufferTyped<Uint2>;
    template class BufferTyped<Uint3>;
    template class BufferTyped<float>;
    template class BufferTyped<Float2>;
    template class BufferTyped<Float3>;
    template class BufferTyped<Float4>;
    template class BufferTyped<Float2x2>;
    template class BufferTyped<Float2x3>;
    template class BufferTyped<Float3x2>;
    template class BufferTyped<Float3x3>;
    template class BufferTyped<Float4x4>;
}