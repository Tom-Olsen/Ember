#include "tripleBuffer.h"
#include "emberMath.h"
#include <string>



namespace emberEngine
{
	// Public methods:
	// Constructor/Destructor:
	template<typename T>
	TripleBuffer<T>::TripleBuffer()
	{

	}
	template<typename T>
	TripleBuffer<T>::TripleBuffer(uint32_t count, const std::string& name, emberCommon::BufferUsage usage)
	{
		for (uint32_t i = 0; i < bufferCount; i++)
			m_buffers[i] = BufferTyped<T>(count, name + "[" + std::to_string(i) + "]", usage);
	}
	template<typename T>
	TripleBuffer<T>::~TripleBuffer()
	{

	}



	// Movable:
	template<typename T>
	TripleBuffer<T>::TripleBuffer(TripleBuffer&& other) noexcept = default;
	template<typename T>
	TripleBuffer<T>& TripleBuffer<T>::operator=(TripleBuffer&& other) noexcept = default;


	// Getters:
	template<typename T>
	std::string TripleBuffer<T>::GetName(uint32_t bufferIndex) const
	{
		return m_buffers[bufferIndex % bufferCount].GetName();
	}
	template<typename T>
	uint64_t TripleBuffer<T>::GetSize(uint32_t bufferIndex) const
	{
		uint32_t wrappedIndex = bufferIndex % bufferCount;
		return m_buffers[wrappedIndex].IsValid() ? m_buffers[wrappedIndex].GetSize() : 0;
	}
	template<typename T>
	uint32_t TripleBuffer<T>::GetCount(uint32_t bufferIndex) const
	{
		uint32_t wrappedIndex = bufferIndex % bufferCount;
		return m_buffers[wrappedIndex].IsValid() ? m_buffers[wrappedIndex].GetCount() : 0;
	}
	template<typename T>
	uint32_t TripleBuffer<T>::GetElementSize() const
	{
		return sizeof(T);
	}
	template<typename T>
	bool TripleBuffer<T>::IsValid() const
	{
		for (uint32_t i = 0; i < bufferCount; i++)
			if (!m_buffers[i].IsValid())
				return false;
		return true;
	}
	template<typename T>
	Buffer& TripleBuffer<T>::GetBuffer(uint32_t bufferIndex)
	{
		return m_buffers[bufferIndex % bufferCount].GetBuffer();
	}
	template<typename T>
	BufferView<T>& TripleBuffer<T>::GetBufferView(uint32_t bufferIndex)
	{
		return m_buffers[bufferIndex % bufferCount].GetBufferView();
	}



	// Upload/Download:
	template<typename T>
	void TripleBuffer<T>::Upload(uint32_t bufferIndex, const void* pSrc, uint64_t count)
	{
		m_buffers[bufferIndex % bufferCount].Upload(pSrc, count);
	}
	template<typename T>
	void TripleBuffer<T>::Upload(uint32_t bufferIndex, const std::vector<T>& src)
	{
		m_buffers[bufferIndex % bufferCount].Upload(src);
	}
	template<typename T>
	void TripleBuffer<T>::Download(uint32_t bufferIndex, void* pDst, uint64_t count)
	{
		m_buffers[bufferIndex % bufferCount].Download(pDst, count);
	}
	template<typename T>
	void TripleBuffer<T>::Download(uint32_t bufferIndex, std::vector<T>& dst)
	{
		m_buffers[bufferIndex % bufferCount].Download(dst);
	}



	// Explicit template instantiations:
	template class TripleBuffer<int>;
	template class TripleBuffer<Int2>;
	template class TripleBuffer<Int3>;
	template class TripleBuffer<uint32_t>;
	template class TripleBuffer<Uint2>;
	template class TripleBuffer<Uint3>;
	template class TripleBuffer<float>;
	template class TripleBuffer<Float2>;
	template class TripleBuffer<Float3>;
	template class TripleBuffer<Float4>;
	template class TripleBuffer<Float2x2>;
	template class TripleBuffer<Float2x3>;
	template class TripleBuffer<Float3x2>;
	template class TripleBuffer<Float3x3>;
	template class TripleBuffer<Float4x4>;
}