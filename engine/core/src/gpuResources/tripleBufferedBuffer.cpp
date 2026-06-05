#include "tripleBufferedBuffer.h"
#include "emberMath.h"
#include <string>



namespace emberEngine
{
	// Public methods:
	// Constructor/Destructor:
	template<typename T>
	TripleBufferedBuffer<T>::TripleBufferedBuffer()
	{

	}
	template<typename T>
	TripleBufferedBuffer<T>::TripleBufferedBuffer(uint32_t count, const std::string& name, emberCommon::BufferUsage usage)
	{
		for (uint32_t i = 0; i < bufferCount; i++)
			m_buffers[i] = BufferTyped<T>(count, name + "[" + std::to_string(i) + "]", usage);
	}
	template<typename T>
	TripleBufferedBuffer<T>::~TripleBufferedBuffer()
	{

	}



	// Movable:
	template<typename T>
	TripleBufferedBuffer<T>::TripleBufferedBuffer(TripleBufferedBuffer&& other) noexcept = default;
	template<typename T>
	TripleBufferedBuffer<T>& TripleBufferedBuffer<T>::operator=(TripleBufferedBuffer&& other) noexcept = default;


	// Getters:
	template<typename T>
	std::string TripleBufferedBuffer<T>::GetName(uint32_t bufferIndex) const
	{
		return m_buffers[bufferIndex % bufferCount].GetName();
	}
	template<typename T>
	uint64_t TripleBufferedBuffer<T>::GetSize(uint32_t bufferIndex) const
	{
		uint32_t wrappedIndex = bufferIndex % bufferCount;
		return m_buffers[wrappedIndex].IsValid() ? m_buffers[wrappedIndex].GetSize() : 0;
	}
	template<typename T>
	uint32_t TripleBufferedBuffer<T>::GetCount(uint32_t bufferIndex) const
	{
		uint32_t wrappedIndex = bufferIndex % bufferCount;
		return m_buffers[wrappedIndex].IsValid() ? m_buffers[wrappedIndex].GetCount() : 0;
	}
	template<typename T>
	uint32_t TripleBufferedBuffer<T>::GetElementSize() const
	{
		return sizeof(T);
	}
	template<typename T>
	bool TripleBufferedBuffer<T>::IsValid() const
	{
		for (uint32_t i = 0; i < bufferCount; i++)
			if (!m_buffers[i].IsValid())
				return false;
		return true;
	}
	template<typename T>
	Buffer& TripleBufferedBuffer<T>::GetBuffer(uint32_t bufferIndex)
	{
		return m_buffers[bufferIndex % bufferCount].GetBuffer();
	}
	template<typename T>
	BufferView<T>& TripleBufferedBuffer<T>::GetBufferView(uint32_t bufferIndex)
	{
		return m_buffers[bufferIndex % bufferCount].GetBufferView();
	}



	// Upload/Download:
	template<typename T>
	void TripleBufferedBuffer<T>::Upload(uint32_t bufferIndex, const void* pSrc, uint64_t count)
	{
		m_buffers[bufferIndex % bufferCount].Upload(pSrc, count);
	}
	template<typename T>
	void TripleBufferedBuffer<T>::Upload(uint32_t bufferIndex, const std::vector<T>& src)
	{
		m_buffers[bufferIndex % bufferCount].Upload(src);
	}
	template<typename T>
	void TripleBufferedBuffer<T>::Download(uint32_t bufferIndex, void* pDst, uint64_t count)
	{
		m_buffers[bufferIndex % bufferCount].Download(pDst, count);
	}
	template<typename T>
	void TripleBufferedBuffer<T>::Download(uint32_t bufferIndex, std::vector<T>& dst)
	{
		m_buffers[bufferIndex % bufferCount].Download(dst);
	}



	// Explicit template instantiations:
	template class TripleBufferedBuffer<int>;
	template class TripleBufferedBuffer<Int2>;
	template class TripleBufferedBuffer<Int3>;
	template class TripleBufferedBuffer<uint32_t>;
	template class TripleBufferedBuffer<Uint2>;
	template class TripleBufferedBuffer<Uint3>;
	template class TripleBufferedBuffer<float>;
	template class TripleBufferedBuffer<Float2>;
	template class TripleBufferedBuffer<Float3>;
	template class TripleBufferedBuffer<Float4>;
	template class TripleBufferedBuffer<Float2x2>;
	template class TripleBufferedBuffer<Float2x3>;
	template class TripleBufferedBuffer<Float3x2>;
	template class TripleBufferedBuffer<Float3x3>;
	template class TripleBufferedBuffer<Float4x4>;
}