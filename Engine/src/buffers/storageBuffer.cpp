#include "storageBuffer.h"
#include "stagingBuffer.h"
#include "emberMath.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include <vulkan/vulkan.h>



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Destructor:
	StorageBuffer::StorageBuffer(uint32_t count, uint32_t elementSize)
	{
		// NOTE:
		// Storage buffers must follow std430 layout alignment rules.
		// Do not have general abstraction for this. Therefore, if you use anything but:
		// int/float/float2/float3/float4/float4x4 you need to add alignment logic here:
		if (elementSize == sizeof(Float3))
			elementSize = sizeof(Float4);

		m_count = count;
		m_elementSize = elementSize;
		m_size = m_count * m_elementSize;

		// Create buffer:
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = m_size;
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_buffer = std::make_unique<VmaBuffer>("storageBuffer", bufferInfo, allocInfo);
	}
	StorageBuffer::~StorageBuffer()
	{

	}



	// Upload/Download:
	void StorageBuffer::Upload(void* pSrc, uint64_t size)
	{
		size = math::Min(size, m_size);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.SetData(pSrc, size);
		stagingBuffer.UploadToBuffer((Buffer*)this, Context::logicalDevice.GetTransferQueue());
	}
	void StorageBuffer::Download(void* pDst, uint64_t size)
	{
		size = math::Min(size, m_size);
		StagingBuffer stagingBuffer(size);
		stagingBuffer.DownloadFromBuffer((Buffer*)this, Context::logicalDevice.GetTransferQueue());
		stagingBuffer.GetData(pDst, size);
	}
}