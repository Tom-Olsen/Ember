#ifndef __INCLUDE_GUARD_renderPass_h__
#define __INCLUDE_GUARD_renderPass_h__
#include "vulkanContext.h"



/// <summary>
/// Polymorphic parent class for all renderPasses.
/// </summary>
class RenderPass
{
public: // Members:
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;

protected: // Members:
	VulkanContext* context;

public: // Methods:
	virtual ~RenderPass();
};



#endif // __INCLUDE_GUARD_renderPass_h__