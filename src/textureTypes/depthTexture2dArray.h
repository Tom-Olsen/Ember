#ifndef __INCLUDE_GUARD_depthTexture2dArray_h__
#define __INCLUDE_GUARD_depthTexture2dArray_h__
#include "texture2d.h"



namespace emberEngine
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= depth, sample
	/// -VkImageCreateFlagBits	= 0
	/// -VkImageViewType		= 2d array
	/// </summary>
	class DepthTexture2dArray : public Texture2d
	{
	public: // Methods:
		DepthTexture2dArray(VulkanContext* pContext, const std::string& name, VkFormat format, int width, int height, uint32_t layerCount);
		~DepthTexture2dArray();
	};
}



#endif // __INCLUDE_GUARD_depthTexture2dArray_h__