#pragma once
#include "vulkanGpuResourceHandle.h"
#include "vulkanRendererExport.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Buffer;



	class VULKAN_RENDERER_API BufferHandle
	{
	private: // Members:
		GpuResourceHandle m_resourceHandle;

	public: // Methods:
		// Constructor/Destructor:
		BufferHandle();
		explicit BufferHandle(const Buffer& buffer);
		~BufferHandle();

		// Copyable:
		BufferHandle(const BufferHandle&) = default;
		BufferHandle& operator=(const BufferHandle&) = default;

		// Movable:
		BufferHandle(BufferHandle&&) noexcept = default;
		BufferHandle& operator=(BufferHandle&&) noexcept = default;

		// Getters:
		Buffer* Get() const;
		Buffer* TryGet() const;
		bool IsValid() const;

		// Comparison operators:
		bool operator==(const BufferHandle& other) const;
		bool operator!=(const BufferHandle& other) const;
	};
}