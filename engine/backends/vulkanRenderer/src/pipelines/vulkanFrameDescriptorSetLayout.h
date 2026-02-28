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
    private:
        static std::vector<UniformBuffer*> s_uniformBuffers;
        static VkDescriptorSetLayout s_descriptorSetLayout;
        static std::vector<VkDescriptorSet> s_descriptorSets;

    public:
        // Init/Clear:
        static void Init();
        static void Clear();

        // Setters:
        static void SetCameraPosition(const Float4& position, uint32_t frameIndex);
        static void SetViewMatrix(const Float4x4& view, uint32_t frameIndex);
        static void SetProjMatrix(const Float4x4& proj, uint32_t frameIndex);
        static void SetWorldToClipMatrix(const Float4x4& w2c, uint32_t frameIndex);

        // Getters:
        static VkDescriptorSetLayout GetVkDescriptorSetLayout();
        static VkDescriptorSet GetVkDescriptorSet(uint32_t frameIndex);

        // Update data:
        static void UpdateShaderData(uint32_t frameIndex);
    };
}