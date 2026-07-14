#include "vulkanSceneDescriptorSetLayout.h"
#include "shadowConstants.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanUniformBuffer.h"
#include "vulkanGarbageCollector.h"
#include <cassert>



namespace vulkanRendererBackend
{
    // Static members:
    std::unique_ptr<UniformBuffer> SceneDescriptorSetLayout::s_pUniformLightPropertiesBuffer;
    VkDescriptorSetLayout SceneDescriptorSetLayout::s_descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> SceneDescriptorSetLayout::s_descriptorSets;
    std::vector<DescriptorSetAllocation> SceneDescriptorSetLayout::s_descriptorSetAllocations;



    // Public methods:
    // Init/Clear:
    void SceneDescriptorSetLayout::Init()
    {
        // Create descriptor set layout:
        {
            // cbuffer LightProperties : register(b2399, SCENE_SET):
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = 2399;
            binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            binding.descriptorCount = 1;
            binding.stageFlags = VK_SHADER_STAGE_ALL;
            binding.pImmutableSamplers = nullptr;

            VkDescriptorSetLayoutCreateInfo createInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            createInfo.bindingCount = 1;
            createInfo.pBindings = &binding;

            VKA(vkCreateDescriptorSetLayout(Context::GetVkDevice(), &createInfo, nullptr, &s_descriptorSetLayout));
            NAME_VK_OBJECT(s_descriptorSetLayout, "DescriptorSetLayout_Scene");
        }

        // Create descriptor sets:
        {
            s_descriptorSets.resize(Context::GetFramesInFlight());
            s_descriptorSetAllocations.reserve(Context::GetFramesInFlight());
            for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
            {
                DescriptorSetAllocation allocation = DescriptorPoolManager::AllocateDescriptorSet(s_descriptorSetLayout, "DescriptorSet_Scene_Frame" + std::to_string(frameIndex));
                s_descriptorSetAllocations.push_back(allocation);
                s_descriptorSets[frameIndex] = allocation.descriptorSet;
            }
        }

        // Create uniform light properties buffer:
        {
            uint32_t offset = 0;
            emberBufferLayout::BufferMember lightDirCount("light_dirCount", offset, sizeof(int));
            offset += sizeof(int);
            emberBufferLayout::BufferMember lightPosCount("light_posCount", offset, sizeof(int));
            offset += sizeof(int);

            emberBufferLayout::BufferMember lightDirectionData("light_directionData", offset, 0);
            uint32_t directionalLightDataSize = sizeof(Float4x4) + sizeof(Float3) + sizeof(int) + sizeof(Float4);
            for (int i = 0; i < MAX_DIR_LIGHTS; i++)
            {
                emberBufferLayout::BufferMember lightDirectionDataElement("light_directionData[" + std::to_string(i) + "]", offset, directionalLightDataSize);
                emberBufferLayout::BufferMember worldToClipMatrix("worldToClipMatrix", offset, sizeof(Float4x4));
                offset += sizeof(Float4x4);
                emberBufferLayout::BufferMember direction("direction", offset, sizeof(Float3));
                offset += sizeof(Float3);
                emberBufferLayout::BufferMember shadowType("shadowType", offset, sizeof(int));
                offset += sizeof(int);
                emberBufferLayout::BufferMember colorIntensity("colorIntensity", offset, sizeof(Float4));
                offset += sizeof(Float4);

                lightDirectionDataElement.AddSubMember(worldToClipMatrix);
                lightDirectionDataElement.AddSubMember(direction);
                lightDirectionDataElement.AddSubMember(shadowType);
                lightDirectionDataElement.AddSubMember(colorIntensity);
                lightDirectionData.AddSubMember(lightDirectionDataElement);
            }
            lightDirectionData.SetSize(offset - lightDirectionData.GetOffset());

            uint32_t positionalLightDataSize = sizeof(Float4x4) + sizeof(Float3) + sizeof(int) + sizeof(Float4) + sizeof(Float2);
            emberBufferLayout::BufferMember lightPositionalData("light_positionData", offset, 0);
            for (int i = 0; i < MAX_POS_LIGHTS; i++)
            {
                emberBufferLayout::BufferMember lightPositionalDataElement("light_positionData[" + std::to_string(i) + "]", offset, positionalLightDataSize);
                emberBufferLayout::BufferMember worldToClipMatrix("worldToClipMatrix", offset, sizeof(Float4x4));
                offset += sizeof(Float4x4);
                emberBufferLayout::BufferMember position("position", offset, sizeof(Float3));
                offset += sizeof(Float3);
                emberBufferLayout::BufferMember shadowType("shadowType", offset, sizeof(int));
                offset += sizeof(int);
                emberBufferLayout::BufferMember colorIntensity("colorIntensity", offset, sizeof(Float4));
                offset += sizeof(Float4);
                emberBufferLayout::BufferMember blendStartEnd("blendStartEnd", offset, sizeof(Float2));
                offset += sizeof(Float2);

                lightPositionalDataElement.AddSubMember(worldToClipMatrix);
                lightPositionalDataElement.AddSubMember(position);
                lightPositionalDataElement.AddSubMember(shadowType);
                lightPositionalDataElement.AddSubMember(colorIntensity);
                lightPositionalDataElement.AddSubMember(blendStartEnd);
                lightPositionalData.AddSubMember(lightPositionalDataElement);
            }
            offset += 8; // tail padding to match the reflected LightProperties cbuffer size.
            lightPositionalData.SetSize(offset - lightPositionalData.GetOffset());

            emberBufferLayout::BufferLayout bufferLayout("LightProperties");
            bufferLayout.AddMember(lightDirCount);
            bufferLayout.AddMember(lightPosCount);
            bufferLayout.AddMember(lightDirectionData);
            bufferLayout.AddMember(lightPositionalData);

            s_pUniformLightPropertiesBuffer = std::make_unique<UniformBuffer>(bufferLayout);
            s_pUniformLightPropertiesBuffer->SetDebugName("UniformBuffer_SceneLightProperties");
        }

        // Bind uniform light properties buffer to descriptor sets:
        for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
        {
            VkDescriptorBufferInfo bufferInfo;
            bufferInfo.buffer = s_pUniformLightPropertiesBuffer->GetVmaBuffer()->GetVkBuffer();
            bufferInfo.offset = i * s_pUniformLightPropertiesBuffer->GetAlignedSubBufferSize();
            bufferInfo.range = s_pUniformLightPropertiesBuffer->GetSize();

            VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = s_descriptorSets[i];
            descriptorWrite.dstBinding = 2399;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;
            descriptorWrite.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
        }
    }
    void SceneDescriptorSetLayout::Clear()
    {
        s_pUniformLightPropertiesBuffer.reset();

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
    void SceneDescriptorSetLayout::SetLightData(const std::vector<emberCommon::DirectionalLight>& directionalLights, uint32_t directionalLightCount, const std::vector<emberCommon::PositionalLight>& positionalLights, uint32_t positionalLightCount)
    {
        SetDirectionalLightData(directionalLights, directionalLightCount);
        SetPositionalLightData(positionalLights, positionalLightCount);
    }



    // Getters:
    VkDescriptorSetLayout SceneDescriptorSetLayout::GetVkDescriptorSetLayout()
    {
        return s_descriptorSetLayout;
    }
    VkDescriptorSet SceneDescriptorSetLayout::GetVkDescriptorSet(uint32_t frameIndex)
    {
        return s_descriptorSets[frameIndex];
    }



    // Update data:
    void SceneDescriptorSetLayout::UpdateShaderData(uint32_t frameIndex)
    {
        s_pUniformLightPropertiesBuffer->UpdateBuffer(frameIndex);
    }



    // Private methods:
    void SceneDescriptorSetLayout::SetDirectionalLightData(const std::vector<emberCommon::DirectionalLight>& directionalLights, uint32_t directionalLightCount)
    {
        assert(directionalLightCount <= directionalLights.size());
        assert(directionalLightCount <= MAX_DIR_LIGHTS);
        s_pUniformLightPropertiesBuffer->SetInt("light_dirCount", static_cast<int>(directionalLightCount));
        static std::string arrayName = "light_directionData";
        for (uint32_t i = 0; i < directionalLightCount; i++)
        {
            s_pUniformLightPropertiesBuffer->SetFloat4x4(arrayName, i, "worldToClipMatrix", directionalLights[i].worldToClipMatrix);
            s_pUniformLightPropertiesBuffer->SetFloat3(arrayName, i, "direction", directionalLights[i].direction);
            s_pUniformLightPropertiesBuffer->SetInt(arrayName, i, "shadowType", static_cast<int>(directionalLights[i].shadowType));
            s_pUniformLightPropertiesBuffer->SetFloat4(arrayName, i, "colorIntensity", Float4(directionalLights[i].color, directionalLights[i].intensity));
        }
    }
    void SceneDescriptorSetLayout::SetPositionalLightData(const std::vector<emberCommon::PositionalLight>& positionalLights, uint32_t positionalLightCount)
    {
        assert(positionalLightCount <= positionalLights.size());
        assert(positionalLightCount <= MAX_POS_LIGHTS);
        s_pUniformLightPropertiesBuffer->SetInt("light_posCount", static_cast<int>(positionalLightCount));
        static std::string arrayName = "light_positionData";
        for (uint32_t i = 0; i < positionalLightCount; i++)
        {
            s_pUniformLightPropertiesBuffer->SetFloat4x4(arrayName, i, "worldToClipMatrix", positionalLights[i].worldToClipMatrix);
            s_pUniformLightPropertiesBuffer->SetFloat3(arrayName, i, "position", positionalLights[i].position);
            s_pUniformLightPropertiesBuffer->SetInt(arrayName, i, "shadowType", static_cast<int>(positionalLights[i].shadowType));
            s_pUniformLightPropertiesBuffer->SetFloat4(arrayName, i, "colorIntensity", Float4(positionalLights[i].color, positionalLights[i].intensity));
            s_pUniformLightPropertiesBuffer->SetFloat2(arrayName, i, "blendStartEnd", Float2(positionalLights[i].blendStart, positionalLights[i].blendEnd));
        }
    }
}