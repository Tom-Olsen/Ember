#pragma once
#include "vulkanTexture2d.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -ImageUsageFlag		= depth, sample <para/>
	/// -ImageCreateFlagBit	= 0 <para/>
	/// -ImageViewType		= 2d array
	/// </summary>
	class DepthTexture2dArray : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor:
		DepthTexture2dArray(const std::string& name, Format format, int width, int height, uint32_t layerCount);
		~DepthTexture2dArray();

		// Non-copyable:
		DepthTexture2dArray(const DepthTexture2dArray&) = delete;
		DepthTexture2dArray& operator=(const DepthTexture2dArray&) = delete;

		// Movable:
		DepthTexture2dArray(DepthTexture2dArray&& other) noexcept = default;
		DepthTexture2dArray& operator=(DepthTexture2dArray&& other) noexcept = default;

		void SetData(void* data) override;
	};
}