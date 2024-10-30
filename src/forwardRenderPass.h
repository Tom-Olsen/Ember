#ifndef __INCLUDE_GUARD_forwardRenderpass_h__
#define __INCLUDE_GUARD_forwardRenderpass_h__
#include <vector>
#include "vulkanContext.h"
#include "vmaImage.h"



/// <summary>
/// Basic forward render pass.
/// </summary>
class ForwardRenderPass
{
public: // Members:
	VkRenderPass renderPass;
	std::unique_ptr<VmaImage> msaaImage;
	std::unique_ptr<VmaImage> depthImage;
	std::vector<VkFramebuffer> framebuffers;

private: // Members:
	VulkanContext* context;
	VkFormat depthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;

public: // Methods:
	ForwardRenderPass(VulkanContext* context, VkSampleCountFlagBits msaaSamples);
	~ForwardRenderPass();

private: // Methods:
	void CreateRenderPass(VkSampleCountFlagBits msaaSamples);
	void CreateMsaaImage();
	void CreateDepthImage();
	void CreateFrameBuffers();
};



#endif // __INCLUDE_GUARD_forwardRenderpass_h__