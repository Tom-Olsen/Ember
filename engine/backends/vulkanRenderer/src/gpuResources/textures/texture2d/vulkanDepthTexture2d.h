#pragma once
#include "vulkanTexture2d.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags	= depth, sample <para/>
	/// -VkImageCreateFlags	= 0 <para/>
	/// -VkImageViewType    = 2d array
	/// </summary>
	class DepthTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		DepthTexture2d(VkFormat format, int width, int height);
		~DepthTexture2d();

		// Non-copyable:
		DepthTexture2d(const DepthTexture2d&) = delete;
		DepthTexture2d& operator=(const DepthTexture2d&) = delete;

		// Movable:
		DepthTexture2d(DepthTexture2d&& other) noexcept = default;
		DepthTexture2d& operator=(DepthTexture2d&& other) noexcept = default;

		void SetData(void* data) override;
	};
}