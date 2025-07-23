#ifndef __INCLUDE_GUARD_storageBuffer_h__
#define __INCLUDE_GUARD_storageBuffer_h__
#include "buffer.h"



namespace emberEngine
{
	/// <summary>
	/// Buffer specialization: <para/>
	/// -VkBufferUsageFlags			= storage, transfer src/dst <para/>
	/// -VmaMemoryUsage				= prefer device <para/>
	/// -VmaAllocationCreateFlags	= 0
	/// </summary>
	class StorageBuffer : public Buffer
	{
	public: // Methods:
		StorageBuffer(uint32_t count, uint32_t elementSize);
		~StorageBuffer();
	};
}



#endif // __INCLUDE_GUARD_storageBuffer_h__