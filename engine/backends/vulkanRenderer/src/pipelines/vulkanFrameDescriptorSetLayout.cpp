#include "vulkanFrameDescriptorSetLayout.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanUniformBuffer.h"
#include "vulkanGarbageCollector.h"



namespace vulkanRendererBackend
{
    // Static members:
    VkDescriptorSetLayout FrameDescriptorSetLayout::s_descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> FrameDescriptorSetLayout::s_descriptorSets;
    std::vector<UniformBuffer*> FrameDescriptorSetLayout::s_uniformBuffers;



    // Public methods:
    // Init/Clear:
    void FrameDescriptorSetLayout::Init()
    {
        // Create descriptor set layout:
        {
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = 1300;
            binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            binding.descriptorCount = 1;
            binding.stageFlags = VK_SHADER_STAGE_ALL;
            binding.pImmutableSamplers = nullptr;

            VkDescriptorSetLayoutCreateInfo createInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            createInfo.bindingCount = 1;
            createInfo.pBindings = &binding;

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
            VKA(vkAllocateDescriptorSets(Context::GetVkDevice(), &allocInfo, s_descriptorSets.data()));
        }

        // Create uniform buffers:
        s_uniformBuffers.resize(Context::GetFramesInFlight());
        for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
        {
            UniformBufferBlock uniformBufferBlock();
            uniformBufferBlock.n
            s_uniformBuffers.emplace_back()
        }

        // --- Create per-frame uniform buffers ---
        s_cpuData.resize(Context::GetFramesInFlight());
        s_dirty.resize(Context::GetFramesInFlight(), true);

        for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
        {
            s_uniformBuffers[i] = new UniformBuffer(sizeof(CameraData));

            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = s_uniformBuffers[i]->GetVkBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(CameraData);

            VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            write.dstSet = s_descriptorSets[i];
            write.dstBinding = 1300;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.descriptorCount = 1;
            write.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &write, 0, nullptr);
        }
    }
    void FrameDescriptorSetLayout::Clear()
    {
        for (UniformBuffer* ub : s_uniformBuffers)
            delete ub;

        vkDestroyDescriptorSetLayout(Context::GetVkDevice(), s_descriptorSetLayout, nullptr);
    }


    // Setters:
    void FrameDescriptorSetLayout::SetCameraPosition(const Float4& position, uint32_t frameIndex)
    {
        s_cpuData[frameIndex].camera_Position = position;
        s_dirty[frameIndex] = true;
    }
    void FrameDescriptorSetLayout::SetViewMatrix(const Float4x4& view, uint32_t frameIndex)
    {
        s_cpuData[frameIndex].camera_viewMatrix = view;
        s_dirty[frameIndex] = true;
    }
    void FrameDescriptorSetLayout::SetProjMatrix(const Float4x4& proj, uint32_t frameIndex)
    {
        s_cpuData[frameIndex].camera_projMatrix = proj;
        s_dirty[frameIndex] = true;
    }
    void FrameDescriptorSetLayout::SetWorldToClipMatrix(const Float4x4& w2c, uint32_t frameIndex)
    {
        s_cpuData[frameIndex].camera_worldToClipMatrix = w2c;
        s_dirty[frameIndex] = true;
    }



    // Getters:
    VkDescriptorSetLayout FrameDescriptorSetLayout::GetVkDescriptorSetLayout()
    {
        return s_descriptorSetLayout;
    }
    VkDescriptorSet FrameDescriptorSetLayout::GetVkDescriptorSet(uint32_t frameIndex)
    {
        return s_descriptorSets[frameIndex];
    }



    // Update data:
    void FrameDescriptorSetLayout::Update(uint32_t frameIndex)
    {
        if (!s_dirty[frameIndex])
            return;

        s_uniformBuffers[frameIndex]->UploadData(&s_cpuData[frameIndex], sizeof(CameraData));
        s_dirty[frameIndex] = false;
    }
}