#ifndef __INCLUDE_GUARD_buffer_h__
#define __INCLUDE_GUARD_buffer_h__
#include "vk_mem_alloc.h"
#include <memory>



namespace emberEngine
{
	// Forward declarations:
	namespace vulkanBackend
	{
		class VmaBuffer;
	}



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
		uint32_t m_count;
		uint32_t m_elementSize;
		uint64_t m_size;	// in bytes: m_size = count * m_elementSize.
		std::unique_ptr<vulkanBackend::VmaBuffer> m_buffer;

	public: // Methods:
		virtual ~Buffer();

		uint64_t GetSize() const;
		uint32_t GetCount() const;
		uint32_t GetElementSize() const;
		vulkanBackend::VmaBuffer* const GetVmaBuffer() const;
		
		// Upload/Download:
		void Upload(VkCommandBuffer commandBuffer, void* pSrc, uint64_t size);
		void Upload(void* pSrc, uint64_t size);
		void Download(VkCommandBuffer commandBuffer, void* pDst, uint64_t size);
		void Download(void* pDst, uint64_t size);
	};
}



#endif // __INCLUDE_GUARD_buffer_h__