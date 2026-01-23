#pragma once
#include <memory>
#include <vector>



// Forward declarations:
typedef struct VkDescriptorSet_T* VkDescriptorSet;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class UniformBuffer;



	// Immutable scene level descriptor set (hard coded to match sceneSet.hlsli):
	class SceneSet
	{
	private: // Members:
		bool m_needsUpdate;
		std::unique_ptr<UniformBuffer> m_pUniformBuffer;
		std::vector<VkDescriptorSet> m_descriptorSets;
		std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;

	public: // Methods:
		// Constructor/Destructor:
		SceneSet(uint32_t maxDirectionalLights, uint32_t maxPositionalLights);
		~SceneSet();

		// Non-copyable:
		SceneSet(const SceneSet& other) = delete;
		SceneSet& operator=(const SceneSet& other) = delete;

		// Movable:
		SceneSet(SceneSet&& other) noexcept;
		SceneSet& operator=(SceneSet&& other) noexcept;

		// Getters:
		void UpdateShaderData(uint32_t frameIndex = -1); // -1 = current frame index.
		const VkDescriptorSet& GetDescriptorSet(uint32_t frameIndex);
		std::vector<VkDescriptorSet>& GetDescriptorSets();
	};
}