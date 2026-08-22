#pragma once
#include "vulkanGpuResourceHandle.h"
#include "vulkanRendererExport.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Texture;



	class VULKAN_RENDERER_API TextureHandle
	{
	private: // Members:
		GpuResourceHandle m_resourceHandle;

	public: // Methods:
		// Constructor/Destructor:
		TextureHandle();
		explicit TextureHandle(const Texture& texture);
		~TextureHandle();

		// Copyable:
		TextureHandle(const TextureHandle&) = default;
		TextureHandle& operator=(const TextureHandle&) = default;

		// Movable:
		TextureHandle(TextureHandle&&) noexcept = default;
		TextureHandle& operator=(TextureHandle&&) noexcept = default;

		// Getters:
		Texture* Get() const;
		Texture* TryGet() const;
		bool IsValid() const;

		// Comparison operators:
		bool operator==(const TextureHandle& other) const;
		bool operator!=(const TextureHandle& other) const;
	};
}