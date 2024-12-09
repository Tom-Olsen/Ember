#ifndef __INCLUDE_GUARD_shadowRenderPass_h__
#define __INCLUDE_GUARD_shadowRenderPass_h__
#include "renderPass.h"
#include <memory>
#include <vulkan/vulkan.h>



class Texture2d;
class VmaImage;
struct VulkanContext;



/// <summary>
/// Shadow render pass.
/// Executed once for every light source.
/// </summary>
class ShadowRenderPass : public RenderPass
{
private: // Members:
	std::unique_ptr<Texture2d> m_shadowMaps;
	VkFormat m_shadowMapFormat = VK_FORMAT_D32_SFLOAT;

public: // Members:
	static uint32_t s_shadowMapWidth;
	static uint32_t s_shadowMapHeight;
	static uint32_t s_layerCount;

public: // Methods:
	ShadowRenderPass(VulkanContext* pContext);
	~ShadowRenderPass();
	Texture2d* const GetShadowMaps() const;

private: // Methods:
	void CreateShadowMapTexture();
	void CreateRenderpass();
	void CreateFramebuffers();
};



#endif // __INCLUDE_GUARD_shadowRenderPass_h__