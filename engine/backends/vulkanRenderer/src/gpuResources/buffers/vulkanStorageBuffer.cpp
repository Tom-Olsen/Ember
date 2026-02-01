#include "vulkanStorageBuffer.h"
#include "vmaBuffer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	StorageBuffer::StorageBuffer(uint32_t count, uint32_t elementSize)
	{
		m_count = count;
		m_elementSize = Std430Alignment(elementSize);
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

		m_pBuffer = std::make_unique<VmaBuffer>(bufferInfo, allocInfo);
	}
	StorageBuffer::~StorageBuffer()
	{

	}



	// Movable:
	StorageBuffer::StorageBuffer(StorageBuffer&&) noexcept = default;
	StorageBuffer& StorageBuffer::operator=(StorageBuffer&&) noexcept = default;



	// Private methods:
	uint32_t StorageBuffer::Std430Alignment(uint32_t elementSize)
	{
	    switch (elementSize)
	    {
	        case sizeof(int):         return sizeof(int); // also works for float as int and float both have size 4.
	        case 2 * sizeof(float):   return 2 * sizeof(float);
	        case 3 * sizeof(float):   return 4 * sizeof(float);
	        case 4 * sizeof(float):   return 4 * sizeof(float);
	        case 16 * sizeof(float):  return 16 * sizeof(float);
	        default: 
	            throw std::runtime_error("StorageBuffer uses unsupported elementSize '" + std::to_string(elementSize) + "' in std430 layout!");
				return 0;
	    }
	}
}