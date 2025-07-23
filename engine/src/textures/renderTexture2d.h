#ifndef __INCLUDE_GUARD_renderTexture2d_h__
#define __INCLUDE_GUARD_renderTexture2d_h__
#include "texture2d.h"



namespace emberEngine
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
		RenderTexture2d(const std::string& name, VkFormat format, int width, int height);
		~RenderTexture2d();

	private: // Methods:
		StagingBuffer* Init(const std::string& name, VkFormat format, const std::filesystem::path& path) override;
	public: // Methods:
		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer) override;
	};
}



#endif // __INCLUDE_GUARD_renderTexture2d_h__