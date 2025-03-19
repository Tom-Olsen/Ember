#ifndef __INCLUDE_GUARD_storageBuffer_h__
#define __INCLUDE_GUARD_storageBuffer_h__
#include "buffer.h"



namespace emberEngine
{
	/// <summary>
	/// Buffer specialization: <para/>
	/// -VkBufferUsageFlags			= storage <para/>
	/// -VmaMemoryUsage				= prefer device <para/>
	/// -VmaAllocationCreateFlags	= 0
	/// </summary>
	class StorageBuffer : public Buffer
	{
	public: // Methods:
		uint32_t m_count;
		uint32_t m_elementSize;
		StorageBuffer(uint32_t count, uint32_t elementSize);
		~StorageBuffer();

		uint32_t GetCount() const;
		uint32_t GetElementSize() const;
	};
}



#endif // __INCLUDE_GUARD_storageBuffer_h__