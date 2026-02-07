#include "vulkanSceneSet.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanGarbageCollector.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanRenderPassManager.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanUniformBuffer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	SceneSet::SceneSet()
	{
		// Create uniform buffer:
		m_needsUpdate = true;
		emberSpirvReflect::UniformBufferBlock* pUniformBufferBlock = new emberSpirvReflect::UniformBufferBlock();
		pUniformBufferBlock->name = "LightProperties";
		m_pUniformBuffer = new UniformBuffer(pUniformBufferBlock);

		// Bindings (hard coded to match sceneSet.hlsli):
		std::vector<VkDescriptorSetLayoutBinding> bindings(1);
		bindings[0].binding = 2300;
		bindings[0].descriptorCount = 1;
		bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		bindings[0].pImmutableSamplers = nullptr;
		bindings[0].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;

		// Descriptor set layout info:
		m_descriptorSetLayouts.resize(Context::GetVkDescriptorPool());
		for (int i = 0; i < Context::GetFramesInFlight(); i++)
		{
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			descriptorSetLayoutCreateInfo.pBindings = bindings.empty() ? nullptr : bindings.data();
			VKA(vkCreateDescriptorSetLayout(Context::GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayouts[i]));
		}

		// Descriptor set allocation info:
		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = Context::GetVkDescriptorPool();
		allocInfo.descriptorSetCount = m_descriptorSetLayouts.size();
		allocInfo.pSetLayouts = m_descriptorSetLayouts.data();

		// Create descriptor set:
		m_descriptorSets.resize(Context::GetVkDescriptorPool());
		VKA(vkAllocateDescriptorSets(Context::GetLogicalDevice()->GetVkDevice(), &allocInfo, m_descriptorSets.data()));

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
	SceneSet::~SceneSet()
	{
		// Queue the destruction of descriptor sets for later collection:
		for (int i = 0; i < Context::GetFramesInFlight(); i++)
		{
			VkDescriptorSet descriptorSet = m_descriptorSets[i];
			GarbageCollector::RecordGarbage([descriptorSet]()
			{
				vkFreeDescriptorSets(Context::GetVkDevice(), Context::GetVkDescriptorPool(), 1, &descriptorSet);
			});
		}
		// Queue the destruction of descriptor set layouts for later collection:
		for (int i = 0; i < Context::GetFramesInFlight(); i++)
		{
			VkDescriptorSetLayout descriptorSetLayout = m_descriptorSetLayouts[i];
			GarbageCollector::RecordGarbage([descriptorSetLayout]()
			{
				vkDestroyDescriptorSetLayout(Context::GetVkDevice(), descriptorSetLayout, nullptr);
			});
		}
	}



	// Movable:
	SceneSet::SceneSet(SceneSet&& other) noexcept = default;
	SceneSet& SceneSet::operator=(SceneSet&& other) noexcept = default;



	// Getters:
	void SceneSet::UpdateShaderData(uint32_t frameIndex)
	{
		// invalid frameIndex (-1 or >= framesInFlight) => use current frameIndex. frameIndex = -1 is default behaviour.
		if (frameIndex == -1 || frameIndex >= Context::GetFramesInFlight())
			frameIndex = Context::GetFrameIndex();

		// Stream uniform buffer data from host memory to GPU only for current frameIndex:
		if (m_needsUpdate)
		{
			uniformBufferBinding.pUniformBuffer->UpdateBuffer();
			m_needsUpdate = false;
		}
	}
	const VkDescriptorSet& SceneSet::GetDescriptorSet(uint32_t frameIndex)
	{
		return m_descriptorSet;
	}
	std::vector<VkDescriptorSet>& SceneSet::GetDescriptorSets()
	{
		return m_descriptorSets;
	}
}