#include "vulkanFrameDescriptorSetLayout.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanUniformBuffer.h"
#include "vulkanGarbageCollector.h"



namespace vulkanRendererBackend
{
    // Static members:
    UniformBuffer FrameDescriptorSetLayout::s_uniformCameraBuffer;
    VkDescriptorSetLayout FrameDescriptorSetLayout::s_descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> FrameDescriptorSetLayout::s_descriptorSets;



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

        // Create uniform camera buffer:
        {
            uint32_t offset = 0;
            emberBufferLayout::BufferMember cameraPosition("camera_Position", offset, sizeof(Float4));
            offset += sizeof(Float4);
            emberBufferLayout::BufferMember cameraViewMatrix("camera_viewMatrix", offset, sizeof(Float4x4));
            offset += sizeof(Float4x4);
            emberBufferLayout::BufferMember cameraProjMatrix("camera_projMatrix", offset, sizeof(Float4x4));
            offset += sizeof(Float4x4);
            emberBufferLayout::BufferMember cameraWorldToClipMatrix("camera_worldToClipMatrix", offset, sizeof(Float4x4));

            emberBufferLayout::BufferLayout bufferLayout("Camera");
            bufferLayout.AddMember(cameraPosition);
            bufferLayout.AddMember(cameraViewMatrix);
            bufferLayout.AddMember(cameraProjMatrix);
            bufferLayout.AddMember(cameraWorldToClipMatrix);

            s_uniformCameraBuffer = UniformBuffer(bufferLayout);
        }

        // Bind uniform light properties buffer to descriptor sets:
        for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
        {
            VkDescriptorBufferInfo bufferInfo;
            bufferInfo.buffer = s_uniformCameraBuffer.GetVmaBuffer()->GetVkBuffer();
            bufferInfo.offset = i * s_uniformCameraBuffer.GetAlignedSubBufferSize();
            bufferInfo.range = s_uniformCameraBuffer.GetSize();

            VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = s_descriptorSets[i];
            descriptorWrite.dstBinding = 1300;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;
            descriptorWrite.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
        }
    }
    void FrameDescriptorSetLayout::Clear()
    {
        vkDestroyDescriptorSetLayout(Context::GetVkDevice(), s_descriptorSetLayout, nullptr);

        // Queue the destruction of each descriptor set for later collection:
        for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
        {
            VkDescriptorSet descriptorSet = s_descriptorSets[i];
            GarbageCollector::RecordGarbage([descriptorSet]()
            {
                vkFreeDescriptorSets(Context::GetVkDevice(), Context::GetVkDescriptorPool(), 1, &descriptorSet);
            });
        }
    }


    // Setters:
    void FrameDescriptorSetLayout::SetCameraData(const Float4& cameraPosition, const Float4x4& viewMatrix, const Float4x4& projMatrix)
    {
        Float4x4 worldToClipMatrix = projMatrix * viewMatrix;
        s_uniformCameraBuffer.SetFloat4("camera_Position", cameraPosition);
        s_uniformCameraBuffer.SetFloat4x4("camera_viewMatrix", viewMatrix);
        s_uniformCameraBuffer.SetFloat4x4("camera_projMatrix", projMatrix);
        s_uniformCameraBuffer.SetFloat4x4("camera_worldToClipMatrix", worldToClipMatrix);
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
    void FrameDescriptorSetLayout::UpdateShaderData(uint32_t frameIndex)
    {
        s_uniformCameraBuffer.UpdateBuffer(frameIndex);
    }
}