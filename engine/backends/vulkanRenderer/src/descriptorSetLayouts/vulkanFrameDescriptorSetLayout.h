#pragma once
#include "emberMath.h"
#include "vulkanDescriptorPoolManager.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // Forward declarations:
    class RenderTargetResources;
    class Texture;
    class UniformBuffer;



    class FrameDescriptorSetLayout
    {
    private: // Members:
        static std::unique_ptr<UniformBuffer> s_pUniformCameraBuffer;
        static VkDescriptorSetLayout s_descriptorSetLayout;
        static std::vector<VkDescriptorSet> s_descriptorSets;
        static std::vector<DescriptorSetAllocation> s_descriptorSetAllocations;

    public: // Methods:
        // Init/Clear:
        static void Init();
        static void Clear();

        // Setters:
        static void SetCameraData(const Float4& cameraPosition, const Float4x4& viewMatrix, const Float4x4& projMatrix);
        static void SetRenderTargetData(uint32_t frameIndex, const RenderTargetResources& renderTargets);
        
        // Getters:
        static VkDescriptorSetLayout GetVkDescriptorSetLayout();
        static VkDescriptorSet GetVkDescriptorSet(uint32_t frameIndex);

        // Update data:
        static void UpdateShaderData(uint32_t frameIndex);

    private: // Methods:
        static void UpdateTextureDescriptor(uint32_t frameIndex, uint32_t binding, const Texture& texture, VkDescriptorType descriptorType, VkImageLayout imageLayout);
    };
}