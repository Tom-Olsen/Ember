#pragma once
#include "vulkanTexture2d.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -ImageUsageFlag		= transfer src (for copying to swapchain), transfer dst, storage, sample, color attachment <para/>
	/// -ImageCreateFlagBit	= 0 <para/>
	/// -ImageViewType		= 2d
	/// </summary>
	class RenderTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		RenderTexture2d(const std::string& name, Format format, int width, int height);
		~RenderTexture2d();

		// Non-copyable:
		RenderTexture2d(const RenderTexture2d&) = delete;
		RenderTexture2d& operator=(const RenderTexture2d&) = delete;

		// Movable:
		RenderTexture2d(RenderTexture2d&& other) noexcept = default;
		RenderTexture2d& operator=(RenderTexture2d&& other) noexcept = default;

		void SetData(void* data) override;
	};
}