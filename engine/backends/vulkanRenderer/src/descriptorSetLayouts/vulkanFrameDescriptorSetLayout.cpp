#include "vulkanFrameDescriptorSetLayout.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include "vulkanDepthTexture2d.h"
#include "vulkanGarbageCollector.h"
#include "vulkanMacros.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSceneColorTexture2dPair.h"
#include "vulkanTexture.h"
#include "vulkanUniformBuffer.h"
#include <array>



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
			// Texture2D<float> sceneDepthTexture : register(t1100, FRAME_SET);
            VkDescriptorSetLayoutBinding sceneDepthBinding{};
            sceneDepthBinding.binding = 1100;
            sceneDepthBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            sceneDepthBinding.descriptorCount = 1;
            sceneDepthBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            sceneDepthBinding.pImmutableSamplers = nullptr;

			// [[vk::image_format("rgba16f")]] RWTexture2D<float4> sceneColorTexture0 : register(u1200, FRAME_SET);
            VkDescriptorSetLayoutBinding sceneColorBinding0{};
            sceneColorBinding0.binding = 1200;
            sceneColorBinding0.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            sceneColorBinding0.descriptorCount = 1;
            sceneColorBinding0.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            sceneColorBinding0.pImmutableSamplers = nullptr;

			// [[vk::image_format("rgba16f")]] RWTexture2D<float4> sceneColorTexture1 : register(u1201, FRAME_SET);
            VkDescriptorSetLayoutBinding sceneColorBinding1{};
            sceneColorBinding1.binding = 1201;
            sceneColorBinding1.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            sceneColorBinding1.descriptorCount = 1;
            sceneColorBinding1.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            sceneColorBinding1.pImmutableSamplers = nullptr;

			// cbuffer CameraProperties : register(b1300, FRAME_SET)
            VkDescriptorSetLayoutBinding cameraBinding{};
            cameraBinding.binding = 1300;
            cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            cameraBinding.descriptorCount = 1;
            cameraBinding.stageFlags = VK_SHADER_STAGE_ALL;
            cameraBinding.pImmutableSamplers = nullptr;

            std::array<VkDescriptorSetLayoutBinding, 4> bindings = { sceneDepthBinding, sceneColorBinding0, sceneColorBinding1, cameraBinding };
            VkDescriptorSetLayoutCreateInfo createInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            createInfo.pBindings = bindings.data();

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
            offset += sizeof(Float4x4);
            emberBufferLayout::BufferMember cameraClipToWorldMatrix("camera_clipToWorldMatrix", offset, sizeof(Float4x4));

            emberBufferLayout::BufferLayout bufferLayout("CameraProperties");
            bufferLayout.AddMember(cameraPosition);
            bufferLayout.AddMember(cameraViewMatrix);
            bufferLayout.AddMember(cameraProjMatrix);
            bufferLayout.AddMember(cameraWorldToClipMatrix);
            bufferLayout.AddMember(cameraClipToWorldMatrix);

            s_pUniformCameraBuffer = std::make_unique<UniformBuffer>(bufferLayout);
            s_pUniformCameraBuffer->SetDebugName("UniformBuffer_FrameCameraProperties");
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
        s_pUniformCameraBuffer.reset();

        vkDestroyDescriptorSetLayout(Context::GetVkDevice(), s_descriptorSetLayout, nullptr);
        s_descriptorSetLayout = VK_NULL_HANDLE;

        // Queue the destruction of each descriptor set for later collection:
        for (const DescriptorSetAllocation& allocation : s_descriptorSetAllocations)
        {
            GarbageCollector::RecordFrameGarbage([allocation]()
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
        Float4x4 clipToWorldMatrix = worldToClipMatrix.Inverse();
        s_pUniformCameraBuffer->SetFloat4("camera_position", cameraPosition);
        s_pUniformCameraBuffer->SetFloat4x4("camera_viewMatrix", viewMatrix);
        s_pUniformCameraBuffer->SetFloat4x4("camera_projMatrix", projMatrix);
        s_pUniformCameraBuffer->SetFloat4x4("camera_worldToClipMatrix", worldToClipMatrix);
        s_pUniformCameraBuffer->SetFloat4x4("camera_clipToWorldMatrix", clipToWorldMatrix);
    }
    void FrameDescriptorSetLayout::SetRenderTargetData(uint32_t frameIndex, SceneColorTexture2dPair& sceneColorTexturePair, DepthTexture2d& sceneDepth)
    {
        UpdateTextureDescriptor(frameIndex, 1100, sceneDepth, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
        UpdateTextureDescriptor(frameIndex, 1200, sceneColorTexturePair.GetRenderTargetTexture(frameIndex, 0), VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_IMAGE_LAYOUT_GENERAL);
        UpdateTextureDescriptor(frameIndex, 1201, sceneColorTexturePair.GetRenderTargetTexture(frameIndex, 1), VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_IMAGE_LAYOUT_GENERAL);
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



    // Private methods:
    void FrameDescriptorSetLayout::UpdateTextureDescriptor(uint32_t frameIndex, uint32_t binding, Texture& texture, VkDescriptorType descriptorType, VkImageLayout imageLayout)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = imageLayout;
        imageInfo.imageView = texture.GetVkImageView();

        VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descriptorWrite.dstSet = s_descriptorSets[frameIndex];
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = descriptorType;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = nullptr;
        descriptorWrite.pImageInfo = &imageInfo;
        descriptorWrite.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}