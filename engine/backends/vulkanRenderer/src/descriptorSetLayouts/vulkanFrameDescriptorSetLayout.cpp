#include "vulkanFrameDescriptorSetLayout.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanUniformBuffer.h"
#include "vulkanGarbageCollector.h"



namespace vulkanRendererBackend
{
    // Static members:
    std::unique_ptr<UniformBuffer> FrameDescriptorSetLayout::s_pUniformCameraBuffer;
    VkDescriptorSetLayout FrameDescriptorSetLayout::s_descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> FrameDescriptorSetLayout::s_descriptorSets;
    std::vector<DescriptorSetAllocation> FrameDescriptorSetLayout::s_descriptorSetAllocations;



    // Public methods:
    // Init/Clear:
    void FrameDescriptorSetLayout::Init()
    {
        // Create descriptor set layout:
        {
            // cbuffer Camera : register(b1399, FRAME_SET):
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = 1399;
            binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            binding.descriptorCount = 1;
            binding.stageFlags = VK_SHADER_STAGE_ALL;
            binding.pImmutableSamplers = nullptr;

            VkDescriptorSetLayoutCreateInfo createInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            createInfo.bindingCount = 1;
            createInfo.pBindings = &binding;

            VKA(vkCreateDescriptorSetLayout(Context::GetVkDevice(), &createInfo, nullptr, &s_descriptorSetLayout));
            NAME_VK_OBJECT(s_descriptorSetLayout, "DescriptorSetLayout_FrameData");
        }

        // Create descriptor sets:
        {
            s_descriptorSets.resize(Context::GetFramesInFlight());
            s_descriptorSetAllocations.reserve(Context::GetFramesInFlight());
            for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
            {
                DescriptorSetAllocation allocation = DescriptorPoolManager::AllocateDescriptorSet(s_descriptorSetLayout, "DescriptorSet_FrameData_Frame" + std::to_string(frameIndex));
                s_descriptorSetAllocations.push_back(allocation);
                s_descriptorSets[frameIndex] = allocation.descriptorSet;
            }
        }

        // Create uniform camera buffer:
        {
            uint32_t offset = 0;
            emberBufferLayout::BufferMember cameraPosition("camera_position", offset, sizeof(Float4));
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

            s_pUniformCameraBuffer = std::make_unique<UniformBuffer>(bufferLayout);
            s_pUniformCameraBuffer->SetDebugName("UniformBuffer_FrameCamera");
        }

        // Bind uniform light properties buffer to descriptor sets:
        for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
        {
            VkDescriptorBufferInfo bufferInfo;
            bufferInfo.buffer = s_pUniformCameraBuffer->GetVmaBuffer()->GetVkBuffer();
            bufferInfo.offset = i * s_pUniformCameraBuffer->GetAlignedSubBufferSize();
            bufferInfo.range = s_pUniformCameraBuffer->GetSize();

            VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = s_descriptorSets[i];
            descriptorWrite.dstBinding = 1399;
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
        s_pUniformCameraBuffer.reset();

        vkDestroyDescriptorSetLayout(Context::GetVkDevice(), s_descriptorSetLayout, nullptr);
        s_descriptorSetLayout = VK_NULL_HANDLE;

        // Queue the destruction of each descriptor set for later collection:
        for (const DescriptorSetAllocation& allocation : s_descriptorSetAllocations)
        {
            GarbageCollector::RecordGarbage([allocation]()
            {
                DescriptorPoolManager::FreeDescriptorSet(allocation);
            });
        }
        s_descriptorSetAllocations.clear();
        s_descriptorSets.clear();
    }


    // Setters:
    void FrameDescriptorSetLayout::SetCameraData(const Float4& cameraPosition, const Float4x4& viewMatrix, const Float4x4& projMatrix)
    {
        Float4x4 worldToClipMatrix = projMatrix * viewMatrix;
        s_pUniformCameraBuffer->SetFloat4("camera_position", cameraPosition);
        s_pUniformCameraBuffer->SetFloat4x4("camera_viewMatrix", viewMatrix);
        s_pUniformCameraBuffer->SetFloat4x4("camera_projMatrix", projMatrix);
        s_pUniformCameraBuffer->SetFloat4x4("camera_worldToClipMatrix", worldToClipMatrix);
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
        s_pUniformCameraBuffer->UpdateBuffer(frameIndex);
    }
}