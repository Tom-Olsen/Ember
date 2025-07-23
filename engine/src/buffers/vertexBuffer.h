#ifndef __INCLUDE_GUARD_vertexBuffer_h__
#define __INCLUDE_GUARD_vertexBuffer_h__
#include "buffer.h"



namespace emberEngine
{
	/// <summary>
	/// Buffer specialization: <para/>
	/// -VkBufferUsageFlags			= vertex, transfer dst <para/>
	/// -VmaMemoryUsage				= prefer device <para/>
	/// -VmaAllocationCreateFlags	= 0
	/// </summary>
	class VertexBuffer : public Buffer
	{
	public: // Methods:
		uint32_t m_count;
		uint32_t m_elementSize;
		VertexBuffer(uint32_t count, uint32_t elementSize);
		~VertexBuffer();
	};
}



#endif // __INCLUDE_GUARD_vertexBuffer_h__