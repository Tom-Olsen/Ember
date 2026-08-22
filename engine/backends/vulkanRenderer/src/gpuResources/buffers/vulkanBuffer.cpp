#include "vulkanBuffer.h"
#include "logger.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include "vulkanGpuResourceRegistry.h"
#include "vulkanLogicalDevice.h"
#include "vulkanStagingBuffer.h"
#include <assert.h>
#include <stdexcept>
#include <utility>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Static members:
	GpuResourceRegistry<Buffer> Buffer::s_resourceRegistry;



	// Public Methods:
	// Constructor/Destructor:
	Buffer::Buffer()
		: m_registrationHandle(s_resourceRegistry.Register(this))
	{

	}
	Buffer::~Buffer()
	{
		UnregisterResource();
	}



	// Movable:
	Buffer::Buffer(Buffer&& other) noexcept
		: m_count(other.m_count)
		, m_elementSize(other.m_elementSize)
		, m_size(other.m_size)
		, m_pBuffer(std::move(other.m_pBuffer))
		, m_registrationHandle(other.m_registrationHandle)
	{
		RebindResource();
		other.m_registrationHandle = GpuResourceHandle();
	}
	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		if (this != &other)
		{
			UnregisterResource();
			m_count = other.m_count;
			m_elementSize = other.m_elementSize;
			m_size = other.m_size;
			m_pBuffer = std::move(other.m_pBuffer);
			m_registrationHandle = other.m_registrationHandle;
			RebindResource();
			other.m_registrationHandle = GpuResourceHandle();
		}
		return *this;
	}



	// Getters:
	uint64_t Buffer::GetSize() const
	{
		return m_size;
	}
	uint32_t Buffer::GetCount() const
	{
		return m_count;
	}
	uint32_t Buffer::GetElementSize() const
	{
		return m_elementSize;
	}
	VmaBuffer* const Buffer::GetVmaBuffer() const
	{
		return m_pBuffer.get();
	}



	// Data transfer:
	void Buffer::Upload(const void* pSrc, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.SetData(pSrc, size);
		stagingBuffer.UploadToBuffer(this, Context::GetLogicalDevice()->GetTransferQueue());
	}
	void Buffer::Download(void* pDst, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.DownloadFromBuffer(this, Context::GetLogicalDevice()->GetTransferQueue());
		stagingBuffer.GetData(pDst, size);
	}



	// Debugging:
	void Buffer::SetDebugName(const std::string& name)
	{
		m_pBuffer->SetDebugName(name);
	}



	// Backend only:
	void Buffer::Upload(VkCommandBuffer vkCommandBuffer, void* pSrc, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.SetData(pSrc, size);
		stagingBuffer.UploadToBuffer(vkCommandBuffer, this);
	}
	void Buffer::Download(VkCommandBuffer vkCommandBuffer, void* pDst, uint64_t size)
	{
		size = std::min(size, m_size);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.DownloadFromBuffer(vkCommandBuffer, this);
		stagingBuffer.GetData(pDst, size);
	}



	// Private methods:
	void Buffer::UnregisterResource()
	{
		if (!m_registrationHandle.IsValid())
			return;
		bool success = s_resourceRegistry.Unregister(m_registrationHandle, this);
		if (!success)
			LOG_ERROR("Buffer::UnregisterResource() failed. Resource handle ({}, {}) is not registered to this buffer.", m_registrationHandle.index, m_registrationHandle.generation);
		assert(success);
		m_registrationHandle = GpuResourceHandle();
	}
	void Buffer::RebindResource()
	{
		if (!m_registrationHandle.IsValid())
			return;
		bool success = s_resourceRegistry.Rebind(m_registrationHandle, this);
		if (!success)
			LOG_ERROR("Buffer::RebindResource() failed. Resource handle ({}, {}) is stale.", m_registrationHandle.index, m_registrationHandle.generation);
		assert(success);
	}
}