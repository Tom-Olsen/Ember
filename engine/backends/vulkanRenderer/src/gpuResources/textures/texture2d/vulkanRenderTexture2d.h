#pragma once
#include "vulkanTexture2d.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src (for copying to swapchain), transfer dst, storage, sample, color attachment <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class RenderTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		RenderTexture2d(const std::string& name, VkFormat format, int width, int height, VkImageUsageFlags usageFlags = 0);
		~RenderTexture2d();

		// Non-copyable:
		RenderTexture2d(const RenderTexture2d&) = delete;
		RenderTexture2d& operator=(const RenderTexture2d&) = delete;

		// Movable:
		RenderTexture2d(RenderTexture2d&& other) noexcept = default;
		RenderTexture2d& operator=(RenderTexture2d&& other) noexcept = default;
	};
}