#include "vulkanStorageBuffer.h"
#include "vmaBuffer.h"
#include "vulkanMacros.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	StorageBuffer::StorageBuffer(uint32_t count, uint32_t elementSize, const std::string& name)
	{
		// NOTE:
		// Storage buffers must follow std430 layout alignment rules.
		// General abstraction not implemented yet. Therefore, if you use anything but:
		// int/float/float2/float3/float4/float4x4 you need to add alignment logic here:
		if (elementSize == 3 * sizeof(float) || elementSize == 4 * sizeof(float))
			elementSize = 4 * sizeof(float);
		else
			throw std::runtime_error("StorageBuffer '" + name + "' uses yet unsuported elementSize!");

		m_name = name;
		m_count = count;
		m_elementSize = elementSize;
		m_size = m_count * m_elementSize;

		// Create buffer:
		BufferCreateInfo bufferInfo = {};
		bufferInfo.size = m_size;
		bufferInfo.usages = BufferUsageFlags::storage_buffer_bit | BufferUsageFlags::transfer_src_bit | BufferUsageFlags::transfer_dst_bit;
		bufferInfo.sharingMode = SharingModes::exclusive;

		AllocationCreateInfo allocInfo = {};
		allocInfo.usages = MemoryUsages::auto_prefer_device;
		allocInfo.flags = 0;
		allocInfo.requiredFlags = 0;
		allocInfo.preferredFlags = 0;

		m_pBuffer = std::make_unique<VmaBuffer>("storageBuffer", bufferInfo, allocInfo);
		NAME_VK_BUFFER(m_pBuffer->GetVkBuffer(), "storageBuffer " + m_name);
	}
	StorageBuffer::~StorageBuffer()
	{

	}



	// Movable:
	StorageBuffer::StorageBuffer(StorageBuffer&&) noexcept = default;
	StorageBuffer& StorageBuffer::operator=(StorageBuffer&&) noexcept = default;
}