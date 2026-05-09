#pragma once
#include "vulkanTexture2d.h"
#include "vulkanRendererExport.h"
#include <string>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags	= transfer src (for copying to swapchain), transfer dst, storage, sample, color attachment <para/>
	/// -VkImageCreateFlags	= 0 <para/>
	/// -VkImageViewType    = 2d
	/// </summary>
	class VULKAN_RENDERER_API RenderTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		RenderTexture2d(Format format, int width, int height);
		~RenderTexture2d();

		// Non-copyable:
		RenderTexture2d(const RenderTexture2d&) = delete;
		RenderTexture2d& operator=(const RenderTexture2d&) = delete;

		// Movable:
		RenderTexture2d(RenderTexture2d&& other) noexcept;
		RenderTexture2d& operator=(RenderTexture2d&& other) noexcept;

		void SetData(void* data) override;
	};
}