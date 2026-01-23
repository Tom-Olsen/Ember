#include "vulkanEngineSet.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanGarbageCollector.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanRenderPassManager.h"
#include "vulkanShadowRenderPass.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	EngineSet::EngineSet(uint32_t maxDirectionalLights, uint32_t maxPositionalLights)
		: m_colorSampler("EngineSetColorSampler"), m_shadowSampler("EngineSetShadowSampler")
	{
		// Bindings (hard coded to match engineSet.hlsli):
		std::vector<VkDescriptorSetLayoutBinding> bindings(3);
		bindings[0].binding = 3000;
		bindings[0].descriptorCount = 1;
		bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		bindings[0].pImmutableSamplers = &m_colorSampler.GetVkSampler();
		bindings[0].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;
		bindings[1].binding = 3001;
		bindings[1].descriptorCount = 1;
		bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		bindings[1].pImmutableSamplers = &m_shadowSampler.GetVkSampler();
		bindings[1].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;
		bindings[2].binding = 3100;
		bindings[2].descriptorCount = maxDirectionalLights + maxPositionalLights;
		bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		bindings[2].pImmutableSamplers = nullptr;
		bindings[2].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;

		// Descriptor set layout info:
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		descriptorSetLayoutCreateInfo.pBindings = bindings.empty() ? nullptr : bindings.data();
		VKA(vkCreateDescriptorSetLayout(Context::GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout));

		// Descriptor set allocation info:
		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = Context::GetVkDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_descriptorSetLayout;

		// Create descriptor set:
		VKA(vkAllocateDescriptorSets(Context::GetLogicalDevice()->GetVkDevice(), &allocInfo, &m_descriptorSet));

		// Bind shadow maps:
		DepthTexture2dArray* pShadowMaps = RenderPassManager::GetShadowRenderPass()->GetShadowMaps();

		std::vector<VkDescriptorImageInfo> imageInfos(maxDirectionalLights + maxPositionalLights);
		for (uint32_t i = 0; i < maxDirectionalLights + maxPositionalLights; i++)
		{
			imageInfos[i].imageLayout = static_cast<VkImageLayout>(pShadowMaps[i].GetVmaImage()->GetImageLayout());
			imageInfos[i].imageView = pShadowMaps[i].GetVmaImage()->GetVkImageView();
		}

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSet;
		descriptorWrite.dstBinding = bindings[2].binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = bindings[2].descriptorType;
		descriptorWrite.descriptorCount = static_cast<uint32_t>(imageInfos.size());
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = imageInfos.data();
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}
	EngineSet::~EngineSet()
	{
		// Queue the destruction of descriptor set for later collection:
		VkDescriptorSet descriptorSet = m_descriptorSet;
		GarbageCollector::RecordGarbage([descriptorSet]()
		{
			vkFreeDescriptorSets(Context::GetVkDevice(), Context::GetVkDescriptorPool(), 1, &descriptorSet);
		});
		// Queue the destruction of descriptor set layout for later collection:
		VkDescriptorSetLayout descriptorSetLayout = m_descriptorSetLayout;
		GarbageCollector::RecordGarbage([descriptorSetLayout]()
		{
			vkDestroyDescriptorSetLayout(Context::GetVkDevice(), descriptorSetLayout, nullptr);
		});
	}



	// Movable:
	EngineSet::EngineSet(EngineSet&& other) noexcept = default;
	EngineSet& EngineSet::operator=(EngineSet&& other) noexcept = default;



	// Getters:
	const VkDescriptorSet& EngineSet::GetDescriptorSet()
	{
		return m_descriptorSet;
	}
}