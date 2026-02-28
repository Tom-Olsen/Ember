#include "vulkanGlobalDescriptorSetLayout.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanGarbageCollector.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanRenderPassManager.h"
#include "vulkanSampler.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanTexture.h"



namespace vulkanRendererBackend
{
    // Static members:
    VkDescriptorSetLayout GlobalDescriptorSetLayout::s_descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> GlobalDescriptorSetLayout::s_descriptorSets;



    // Public Methods:
    // Init/Clear:
    void GlobalDescriptorSetLayout::Init() // Ember::ToDo: call Init() and Clear() at appropriate locations in Context constructor/destructor.
    {
        // Create descriptor set layout:
        {
            std::array<VkDescriptorSetLayoutBinding, 3> bindings{};

            // SamplerState colorSampler : register(s3000)
            bindings[0].binding = 3000;
            bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            bindings[0].descriptorCount = 1;
            bindings[0].stageFlags = VK_SHADER_STAGE_ALL; // or restrict if desired
            bindings[0].pImmutableSamplers = &DefaultGpuResources::GetColorSampler()->GetVkSampler();

            // SamplerComparisonState shadowSampler : register(s3001)
            bindings[1].binding = 3001;
            bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            bindings[1].descriptorCount = 1;
            bindings[1].stageFlags = VK_SHADER_STAGE_ALL;
            bindings[1].pImmutableSamplers = &DefaultGpuResources::GetShadowSampler()->GetVkSampler();

            // Texture2DArray<float> shadowMaps : register(t3100)
            bindings[2].binding = 3100;
            bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            bindings[2].descriptorCount = 1;
            bindings[2].stageFlags = VK_SHADER_STAGE_ALL;
            bindings[2].pImmutableSamplers = nullptr;

            VkDescriptorSetLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            createInfo.pBindings = bindings.data();

            VKA(vkCreateDescriptorSetLayout(Context::GetVkDevice(), &createInfo, nullptr, &s_descriptorSetLayout));
        }

        // Create descriptor sets:
        {
            std::vector<VkDescriptorSetLayout> layouts(Context::GetFramesInFlight(), s_descriptorSetLayout); // same desciptorSetLayout for all frames.

            VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
            allocInfo.descriptorPool = Context::GetVkDescriptorPool();
            allocInfo.descriptorSetCount = Context::GetFramesInFlight();
            allocInfo.pSetLayouts = layouts.data();

            s_descriptorSets.resize(Context::GetFramesInFlight());
            VKA(vkAllocateDescriptorSets(Context::GetLogicalDevice()->GetVkDevice(), &allocInfo, s_descriptorSets.data()));
        }

        // Bind shadow maps to descriptor sets:
        ShadowRenderPass* pShadowRenderPass = RenderPassManager::GetShadowRenderPass();
        Texture* pShadowMaps = static_cast<Texture*>(pShadowRenderPass->GetShadowMaps());
        for (int i = 0; i < Context::GetFramesInFlight(); i++)
        {
            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = static_cast<VkImageLayout>(pShadowMaps->GetVmaImage()->GetImageLayout());
            imageInfo.imageView = pShadowMaps->GetVmaImage()->GetVkImageView();

            VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = s_descriptorSets[i];
            descriptorWrite.dstBinding = 3100;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = nullptr;
            descriptorWrite.pImageInfo = &imageInfo;
            descriptorWrite.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
        }
    }
    void GlobalDescriptorSetLayout::Clear()
    {
        vkDestroyDescriptorSetLayout(Context::GetVkDevice(), s_descriptorSetLayout, nullptr);

        // Queue the destruction of each descriptor set for later collection:
        for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
        {
            VkDescriptorSet descriptorSet = m_descriptorSets[i];
            GarbageCollector::RecordGarbage([descriptorSet]()
            {
                vkFreeDescriptorSets(Context::GetVkDevice(), Context::GetVkDescriptorPool(), 1, &descriptorSet);
            });
        }
    }



    // Getters:
    VkDescriptorSetLayout GlobalDescriptorSetLayout::GetVkDescriptorSetLayout()
    {
        return s_descriptorSetLayout;
    }
    VkDescriptorSet GlobalDescriptorSetLayout::GetVkDescriptorSet(uint32_t frameIndex)
    {
        return s_descriptorSets[frameIndex];
    }
}