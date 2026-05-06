#pragma once
#include "commonLighting.h"
#include "emberMath.h"
#include <memory>
#include <vulkan/vulkan.h>
#include <vector>



namespace vulkanRendererBackend
{
    // Forward declerations:
    class UniformBuffer;



    class SceneDescriptorSetLayout
    {
    private: // Members:
        static std::unique_ptr<UniformBuffer> s_pUniformLightPropertiesBuffer;
        static VkDescriptorSetLayout s_descriptorSetLayout;
        static std::vector<VkDescriptorSet> s_descriptorSets;

    public: // Methods:
        // Init/Clear:
        static void Init();
        static void Clear();

        // Setters:
        static void SetLightData(const std::vector<emberCommon::DirectionalLight>& directionalLights, uint32_t directionalLightCount, const std::vector<emberCommon::PositionalLight>& positionalLights, uint32_t positionalLightCount);

        // Getters:
        static VkDescriptorSetLayout GetVkDescriptorSetLayout();
        static VkDescriptorSet GetVkDescriptorSet(uint32_t frameIndex);

        // Update data:
        static void UpdateShaderData(uint32_t frameIndex);

    private: // Methods:
        static void SetDirectionalLightData(const std::vector<emberCommon::DirectionalLight>& directionalLights, uint32_t directionalLightCount);
        static void SetPositionalLightData(const std::vector<emberCommon::PositionalLight>& positionalLights, uint32_t positionalLightCount);
    };
}