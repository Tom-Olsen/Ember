#ifndef __INCLUDE_GUARD_buffer_h__
#define __INCLUDE_GUARD_buffer_h__
#include "vk_mem_alloc.h"
#include <memory>



namespace emberEngine
{
	// Forward declarations:
	class VmaBuffer;



	/// <summary>
	/// Polymorphic parent class for different kinds of gpu buffers.
	/// Each derivative handles a different type of VkBuffer.
	/// The buffers differ in: <para/>
	/// -VkBufferUsageFlags			=> uniform, storage, ... <para/>
	/// -VmaMemoryUsage				=> prefer host/device, ... <para/>
	/// -VmaAllocationCreateFlags	=> mapped, host access, ...
	/// </summary>
	class Buffer
	{
	protected: // Members:
		uint32_t m_size;	// in bytes: m_size = count * sizeof(type).
		std::unique_ptr<VmaBuffer> m_buffer;

	public: // Methods:
		 virtual ~Buffer();

		uint32_t GetSize() const;
		const VmaBuffer* const GetVmaBuffer() const;
	};
}



#endif // __INCLUDE_GUARD_buffer_h__