#pragma once
#include "vulkanBuffer.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Buffer specialization: <para/>
	/// -VkBufferUsageFlags			= vertex, transfer dst <para/>
	/// -VmaMemoryUsage				= prefer device <para/>
	/// -VmaAllocationCreateFlags	= 0
	/// </summary>
	class VertexBuffer : public Buffer
	{
	public: // Members:
		uint32_t m_count;
		uint32_t m_elementSize;

	public: // Methods:
		VertexBuffer(uint32_t count, uint32_t elementSize, std::string name);
		~VertexBuffer();

		// Non-copyable:
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		// Movable:
		VertexBuffer(VertexBuffer&&) noexcept = default;
		VertexBuffer& operator=(VertexBuffer&&) noexcept = default;
	};
}