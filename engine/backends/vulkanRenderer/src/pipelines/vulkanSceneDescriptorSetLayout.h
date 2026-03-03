#pragma once
#include "commonLighting.h"
#include "emberMath.h"
#include <vulkan/vulkan.h>
#include <vector>



namespace vulkanRendererBackend
{
    // Forward declerations:
    class UniformBuffer;



    class SceneDescriptorSetLayout
    {
    private: // Members:
        static UniformBuffer s_uniformLightPropertiesBuffer;
        static VkDescriptorSetLayout s_descriptorSetLayout;
        static std::vector<VkDescriptorSet> s_descriptorSets;

    public: // Methods:
        // Init/Clear:
        static void Init();
        static void Clear();

        // Setters:
        static void SetLightData(std::vector<emberCommon::DirectionalLight>& directionalLights, std::vector<emberCommon::PositionalLight>& positionalLights);

        // Getters:
        static VkDescriptorSetLayout GetVkDescriptorSetLayout();
        static VkDescriptorSet GetVkDescriptorSet(uint32_t frameIndex);

        // Update data:
        static void UpdateShaderData(uint32_t frameIndex);

    private: // Methods:
        static void SetDirectionalLightData(std::vector<emberCommon::DirectionalLight>& directionalLights);
        static void SetPositionalLightData(std::vector<emberCommon::PositionalLight>& positionalLights);
    };
}