#ifndef __INCLUDE_GUARD_shadingRenderpass_h__
#define __INCLUDE_GUARD_shadingRenderpass_h__
#include <vector>
#include "renderPass.h"
#include "vulkanContext.h"
#include "vmaImage.h"



/// <summary>
/// Basic shading render pass.
/// </summary>
class ShadingRenderPass : public RenderPass
{
public: // Members:
	std::unique_ptr<VmaImage> msaaImage;
	std::unique_ptr<VmaImage> depthImage;

private: // Members:
	VkFormat depthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;

public: // Methods:
	ShadingRenderPass(VulkanContext* context);
	~ShadingRenderPass();

private: // Methods:
	void CreateRenderPass();
	void CreateMsaaImage();
	void CreateDepthImage();
	void CreateFrameBuffers();
};



#endif // __INCLUDE_GUARD_shadingRenderpass_h__