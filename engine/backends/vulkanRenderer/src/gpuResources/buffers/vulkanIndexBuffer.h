#pragma once
#include "vulkanBuffer.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Buffer specialization: <para/>
	/// -VkBufferUsageFlags			= index, transfer dst <para/>
	/// -VmaMemoryUsage				= prefer device <para/>
	/// -VmaAllocationCreateFlags	= 0
	/// </summary>
	class IndexBuffer : public Buffer
	{
	public: // Methods:
		// Constructor/Destructor:
		IndexBuffer(uint32_t count, uint32_t elementSize, std::string name);
		~IndexBuffer();

		// Non-copyable:
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;

		// Movable:
		IndexBuffer(IndexBuffer&&) noexcept = default;
		IndexBuffer& operator=(IndexBuffer&&) noexcept = default;
	};
}