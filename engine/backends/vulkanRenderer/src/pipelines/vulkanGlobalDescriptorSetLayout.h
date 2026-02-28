#pragma once
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward decleration:
	class Texture;



	class GlobalDescriptorSetLayout
	{
	private: // Members:
		static VkDescriptorSetLayout s_descriptorSetLayout;
		static std::vector<VkDescriptorSet> s_descriptorSets;

	public: // Methods:
		// Init/Clear:
		static void Init();
		static void Clear();

		// Getters:
		static VkDescriptorSetLayout GetVkDescriptorSetLayout();
		static VkDescriptorSet GetVkDescriptorSet(uint32_t frameIndex)
	};
}