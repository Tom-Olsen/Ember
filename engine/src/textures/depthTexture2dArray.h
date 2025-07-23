#ifndef __INCLUDE_GUARD_depthTexture2dArray_h__
#define __INCLUDE_GUARD_depthTexture2dArray_h__
#include "texture2d.h"



namespace emberEngine
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

	private: // Methods:
		StagingBuffer* Init(const std::string& name, VkFormat format, const std::filesystem::path& path) override;
	public: // Methods:
		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer) override;
	};
}



#endif // __INCLUDE_GUARD_depthTexture2dArray_h__