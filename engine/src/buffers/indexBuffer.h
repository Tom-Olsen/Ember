#ifndef __INCLUDE_GUARD_imageBuffer_h__
#define __INCLUDE_GUARD_imageBuffer_h__
#include "buffer.h"



namespace emberEngine
{
	/// <summary>
	/// Buffer specialization: <para/>
	/// -VkBufferUsageFlags			= index, transfer dst <para/>
	/// -VmaMemoryUsage				= prefer device <para/>
	/// -VmaAllocationCreateFlags	= 0
	/// </summary>
	class IndexBuffer : public Buffer
	{
	public: // Methods:
		uint32_t m_count;
		uint32_t m_elementSize;
		IndexBuffer(uint32_t count, uint32_t elementSize, std::string name);
		~IndexBuffer();
	};
}



#endif // __INCLUDE_GUARD_imageBuffer_h__