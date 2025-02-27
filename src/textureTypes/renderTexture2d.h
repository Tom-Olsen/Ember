#ifndef __INCLUDE_GUARD_renderTexture2d_h__
#define __INCLUDE_GUARD_renderTexture2d_h__
#include "texture2d.h"



namespace emberEngine
{
	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src (for mipmap creation), transfer dst, storage, sample <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class RenderTexture2d : public Texture2d
	{
	public: // Methods:
		RenderTexture2d(VulkanContext* pContext, const std::string& name, VkFormat format, int width, int height);
		~RenderTexture2d();
	};
}



#endif // __INCLUDE_GUARD_renderTexture2d_h__