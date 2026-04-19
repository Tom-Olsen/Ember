#pragma once
#include "emberMath.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // Forward declarations:
    class UniformBuffer;



    class FrameDescriptorSetLayout
    {
    private: // Members:
        static std::unique_ptr<UniformBuffer> s_pUniformCameraBuffer;
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