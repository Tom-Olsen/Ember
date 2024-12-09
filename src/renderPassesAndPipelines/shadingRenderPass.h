#ifndef __INCLUDE_GUARD_shadingRenderpass_h__
#define __INCLUDE_GUARD_shadingRenderpass_h__
#include "renderPass.h"
#include <memory>
#include <vulkan/vulkan.h>



class VmaImage;
struct VulkanContext;



/// <summary>
/// Basic shading render pass.
/// </summary>
class ShadingRenderPass : public RenderPass
{
private: // Members:
	std::unique_ptr<VmaImage> m_msaaImage;
	std::unique_ptr<VmaImage> m_depthImage;
	VkFormat m_depthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;

public: // Methods:
	ShadingRenderPass(VulkanContext* pContext);
	~ShadingRenderPass();
	const VmaImage* const GetMsaaVmaImage() const;
	const VmaImage* const GetDepthVmaImage() const;

private: // Methods:
	void CreateRenderPass();
	void CreateMsaaImage();
	void CreateDepthImage();
	void CreateFrameBuffers();
};



#endif // __INCLUDE_GUARD_shadingRenderpass_h__