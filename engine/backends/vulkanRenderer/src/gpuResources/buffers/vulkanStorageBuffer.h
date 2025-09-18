#pragma once
#include "vulkanBuffer.h"



namespace vulkanRendererBackend
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
		// Constructor/Destructor:
		StorageBuffer(uint32_t count, uint32_t elementSize, std::string name);
		~StorageBuffer();

		// Non-copyable:
		StorageBuffer(const StorageBuffer&) = delete;
		StorageBuffer& operator=(const StorageBuffer&) = delete;

		// Movable:
		StorageBuffer(StorageBuffer&&) noexcept = default;
		StorageBuffer& operator=(StorageBuffer&&) noexcept = default;
	};
}