#ifndef __INCLUDE_GUARD_vulkanRendererBackend_buffer_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_buffer_h__
#include "iBuffer.h"
#include "vk_mem_alloc.h"
#include <memory>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
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
	class Buffer : emberBackendInterface::IBuffer
	{
	protected: // Members:
		std::string m_name;
		uint32_t m_count;
		uint32_t m_elementSize;
		uint64_t m_size;		// in bytes: m_size = count * m_elementSize.
		std::unique_ptr<VmaBuffer> m_pBuffer;

	public: // Methods:
		Buffer() = default;
		virtual ~Buffer();

		// Non-copyable:
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		// Movable:
		Buffer(Buffer&&) noexcept = default;
		Buffer& operator=(Buffer&&) noexcept = default;

		// Getters:
		std::string GetName() const override;
		uint64_t GetSize() const override;
		uint32_t GetCount() const override;
		uint32_t GetElementSize() const override;
		VmaBuffer* const GetVmaBuffer() const;
		
		// Upload/Download:
		void Upload(VkCommandBuffer vkCommandBuffer, void* pSrc, uint64_t size);
		void Upload(const void* pSrc, uint64_t size) override;
		void Download(VkCommandBuffer vkCommandBuffer, void* pDst, uint64_t size);
		void Download(void* pDst, uint64_t size) override;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_buffer_h__