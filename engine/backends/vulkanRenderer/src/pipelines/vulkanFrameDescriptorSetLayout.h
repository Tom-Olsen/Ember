#pragma once
#include "emberMath.h"
#include <vulkan/vulkan.h>
#include <vector>



namespace vulkanRendererBackend
{
    // Forward declerations:
    class UniformBuffer;



    class FrameDescriptorSetLayout
    {
    private: // Members:
        static UniformBuffer s_uniformCameraBuffer;
        static VkDescriptorSetLayout s_descriptorSetLayout;
        static std::vector<VkDescriptorSet> s_descriptorSets;

    public: // Methods:
        // Init/Clear:
        static void Init();
        static void Clear();

        // Setters:
        static void SetCameraData(const Float4& cameraPosition, const Float4x4& viewMatrix, const Float4x4& projMatrix);
        
        // Getters:
        static VkDescriptorSetLayout GetVkDescriptorSetLayout();
        static VkDescriptorSet GetVkDescriptorSet(uint32_t frameIndex);

        // Update data:
        static void UpdateShaderData(uint32_t frameIndex);
    };
}