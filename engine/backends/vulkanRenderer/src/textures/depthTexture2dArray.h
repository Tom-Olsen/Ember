#ifndef __INCLUDE_GUARD_vulkanRendererBackend_depthTexture2dArray_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_depthTexture2dArray_h__
#include "texture2d.h"



namespace vulkanRendererBackend
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= depth, sample <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d array
	/// </summary>
	class DepthTexture2dArray : public Texture2d
	{
	public: // Methods:
		DepthTexture2dArray(const std::string& name, VkFormat format, int width, int height, uint32_t layerCount);
		~DepthTexture2dArray();

		// Non-copyable:
		DepthTexture2dArray(const DepthTexture2dArray&) = delete;
		DepthTexture2dArray& operator=(const DepthTexture2dArray&) = delete;

		// DepthTexture2dArray:
		DepthTexture2dArray(DepthTexture2dArray&& other) noexcept = default;
		DepthTexture2dArray& operator=(DepthTexture2dArray&& other) noexcept = default;

		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer) override;

	private: // Methods:
		StagingBuffer* Load(const std::string& name, VkFormat format, const std::filesystem::path& path) override;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_depthTexture2dArray_h__