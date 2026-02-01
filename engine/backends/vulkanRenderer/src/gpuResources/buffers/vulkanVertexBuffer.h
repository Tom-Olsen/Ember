#pragma once
#include "vulkanBuffer.h"
#include "vulkanRendererExport.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Buffer specialization: <para/>
	/// -VkBufferUsageFlags			= vertex, transfer dst <para/>
	/// -VmaMemoryUsage				= prefer device <para/>
	/// -VmaAllocationCreateFlags	= 0
	/// </summary>
	class VULKAN_RENDERER_API VertexBuffer : public Buffer
	{
	public: // Methods:
		// Constructor/Destructor:
		VertexBuffer(uint32_t count, uint32_t elementSize);
		~VertexBuffer();

		// Non-copyable:
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		// Movable:
		VertexBuffer(VertexBuffer&&) noexcept;
		VertexBuffer& operator=(VertexBuffer&&) noexcept;
	};
}