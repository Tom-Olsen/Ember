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



    // Public methods:
    // Init/Clear:
    void SceneDescriptorSetLayout::Init()
    {
        // Create descriptor set layout:
        {
            // cbuffer LightProperties : register(b2300, SCENE_SET):
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = 2300;
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

        // Create uniform light properties buffer:
        {
            uint32_t offset = 0;
            emberBufferLayout::BufferMember lightDirCount("light_dirCount", offset, sizeof(int));
            offset += sizeof(int);
            emberBufferLayout::BufferMember lightPosCount("light_posCount", offset, sizeof(int));
            offset += 8; // +8 due to alignment rules.

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

            uint32_t positionalLightDataSize = sizeof(Float4x4) + sizeof(Float3) + sizeof(int) + sizeof(Float4) + sizeof(Float2) + 8; // +8 due to alignment rules.
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
                emberBufferLayout::BufferMember blendStartEnd("blendStartEnd", offset, sizeof(Float4));
                offset += sizeof(Float2) + 8; // +8 due to alignment rules.

                lightPositionalDataElement.AddSubMember(worldToClipMatrix);
                lightPositionalDataElement.AddSubMember(position);
                lightPositionalDataElement.AddSubMember(shadowType);
                lightPositionalDataElement.AddSubMember(colorIntensity);
                lightPositionalDataElement.AddSubMember(blendStartEnd);
                lightPositionalData.AddSubMember(lightPositionalDataElement);
            }
            lightPositionalData.SetSize(offset - lightPositionalData.GetOffset());

            emberBufferLayout::BufferLayout bufferLayout("LightProperties");
            bufferLayout.AddMember(lightDirCount);
            bufferLayout.AddMember(lightPosCount);
            bufferLayout.AddMember(lightDirectionData);
            bufferLayout.AddMember(lightPositionalData);

            s_pUniformLightPropertiesBuffer = std::make_unique<UniformBuffer>(bufferLayout);
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
            descriptorWrite.dstBinding = 2300;
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
    void SceneDescriptorSetLayout::SetLightData(std::vector<emberCommon::DirectionalLight>& directionalLights, std::vector<emberCommon::PositionalLight>& positionalLights)
    {
        SetDirectionalLightData(directionalLights);
        SetPositionalLightData(positionalLights);
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
    void SceneDescriptorSetLayout::SetDirectionalLightData(std::vector<emberCommon::DirectionalLight>& directionalLights)
    {
        assert(directionalLights.size() <= MAX_DIR_LIGHTS);
        s_pUniformLightPropertiesBuffer->SetInt("light_dirCount", static_cast<int>(directionalLights.size()));
        static std::string arrayName = "light_directionData";
        for (uint32_t i = 0; i < directionalLights.size(); i++)
        {
            s_pUniformLightPropertiesBuffer->SetFloat4x4(arrayName, i, "worldToClipMatrix", directionalLights[i].worldToClipMatrix);
            s_pUniformLightPropertiesBuffer->SetFloat3(arrayName, i, "direction", directionalLights[i].direction);
            s_pUniformLightPropertiesBuffer->SetInt(arrayName, i, "shadowType", static_cast<int>(directionalLights[i].shadowType));
            s_pUniformLightPropertiesBuffer->SetFloat4(arrayName, i, "colorIntensity", Float4(directionalLights[i].color, directionalLights[i].intensity));
        }
    }
    void SceneDescriptorSetLayout::SetPositionalLightData(std::vector<emberCommon::PositionalLight>& positionalLights)
    {
        assert(positionalLights.size() <= MAX_POS_LIGHTS);
        s_pUniformLightPropertiesBuffer->SetInt("light_posCount", static_cast<int>(positionalLights.size()));
        static std::string arrayName = "light_positionData";
        for (uint32_t i = 0; i < positionalLights.size(); i++)
        {
            s_pUniformLightPropertiesBuffer->SetFloat4x4(arrayName, i, "worldToClipMatrix", positionalLights[i].worldToClipMatrix);
            s_pUniformLightPropertiesBuffer->SetFloat3(arrayName, i, "position", positionalLights[i].position);
            s_pUniformLightPropertiesBuffer->SetInt(arrayName, i, "shadowType", static_cast<int>(positionalLights[i].shadowType));
            s_pUniformLightPropertiesBuffer->SetFloat4(arrayName, i, "colorIntensity", Float4(positionalLights[i].color, positionalLights[i].intensity));
            s_pUniformLightPropertiesBuffer->SetFloat2(arrayName, i, "blendStartEnd", Float2(positionalLights[i].blendStart, positionalLights[i].blendEnd));
        }
    }
}