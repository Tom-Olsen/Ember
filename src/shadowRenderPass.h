#pragma once
#ifndef __INCLUDE_GUARD_shadowRenderPass_h__
#define __INCLUDE_GUARD_shadowRenderPass_h__
#include <vector>
#include "vulkanContext.h"
#include "texture2d.h"



/// <summary>
/// Shadow render pass.
/// Executed once for every light source.
/// </summary>
class ShadowRenderPass
{
private: // Members:

public: // Members:
	VkRenderPass renderPass;
	std::unique_ptr<Texture2d> shadowMap;
	std::vector<VkFramebuffer> framebuffers;

	static uint32_t shadowMapWidth;
	static uint32_t shadowMapHeight;

private: // Members:
	VulkanContext* context;
	VkFormat shadowMapFormat = VK_FORMAT_D32_SFLOAT;

public: // Methods:
	ShadowRenderPass(VulkanContext* context);
	~ShadowRenderPass();

private: // Methods:
	void CreateShadowMapTexture();
	void CreateRenderpass();
	void CreateFramebuffers();
};



#endif // __INCLUDE_GUARD_shadowRenderPass_h__