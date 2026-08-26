#pragma once
#include "vulkanTexture2d.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags = sample, color attachment <para/>
	/// -VkImageCreateFlags = 0 <para/>
	/// -VkImageViewType = 2d
	/// </summary>
	class GBufferTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		GBufferTexture2d(VkFormat format, int width, int height);
		~GBufferTexture2d();

		// Non-copyable:
		GBufferTexture2d(const GBufferTexture2d&) = delete;
		GBufferTexture2d& operator=(const GBufferTexture2d&) = delete;

		// Movable:
		GBufferTexture2d(GBufferTexture2d&& other) noexcept = default;
		GBufferTexture2d& operator=(GBufferTexture2d&& other) noexcept = default;

		void SetData(void* data) override;
	};
}