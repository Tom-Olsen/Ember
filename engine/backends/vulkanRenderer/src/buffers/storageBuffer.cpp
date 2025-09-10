#include "storageBuffer.h"
#include "vmaBuffer.h"
#include "vulkanMacros.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	StorageBuffer::StorageBuffer(uint32_t count, uint32_t elementSize, std::string name)
	{
		// NOTE:
		// Storage buffers must follow std430 layout alignment rules.
		// General abstraction not implemented yet. Therefore, if you use anything but:
		// int/float/float2/float3/float4/float4x4 you need to add alignment logic here:
		if (elementSize == 3 * sizeof(float))
			elementSize = 4 * sizeof(float);

		m_count = count;
		m_elementSize = elementSize;
		m_size = m_count * m_elementSize;
		m_name = name;

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

		m_pBuffer = std::make_unique<VmaBuffer>("storageBuffer", bufferInfo, allocInfo);
		NAME_VK_BUFFER(m_pBuffer->GetVkBuffer(), "storageBuffer " + m_name);
	}
	StorageBuffer::~StorageBuffer()
	{

	}
}