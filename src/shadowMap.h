#pragma once
#ifndef __INCLUDE_GUARD_shadowMap_h__
#define __INCLUDE_GUARD_shadowMap_h__
#include <vector>
#include <filesystem>
#include "vulkanContext.h"
#include "vulkanUniformBuffer.h"
#include "vmaImage.h"




class ShadowMap
{
private: // Members:

public: // Members:
	std::unique_ptr<VmaImage> image;
	std::vector<VkFramebuffer> framebuffers;
	VkRenderPass renderpass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	std::vector<VkDescriptorSet> descriptorSets;

	VkFormat shadowFormat;
	static uint32_t shadowMapWidth;
	static uint32_t shadowMapHeight;

private: // Members:
	VulkanContext* context;
	std::unique_ptr<VulkanUniformBuffer> uniformBuffer;

public: // Methods:
	ShadowMap(VulkanContext* context);
	~ShadowMap();

private: // Methods:
	void CreateImage();
	void CreateRenderpass();
	void CreateFramebuffers();
	void CreatePipelineLayout();
	void CreatePipeline();
	void CreateDescriptorSets();
	void UpdateDescriptorSets();
	std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile);
};



#endif // __INCLUDE_GUARD_shadowMap_h__