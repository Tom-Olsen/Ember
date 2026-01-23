#pragma once
#include "vulkanColorSampler.h"
#include "vulkanShadowSampler.h"



// Forward declarations:
typedef struct VkDescriptorSet_T* VkDescriptorSet;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;



namespace vulkanRendererBackend
{
	// Immutable engine level descriptor set (hard coded to match engineSet.hlsli):
	class EngineSet
	{
	private: // Members:
		VkDescriptorSet m_descriptorSet;
		VkDescriptorSetLayout m_descriptorSetLayout;
		ColorSampler m_colorSampler;
		ShadowSampler m_shadowSampler;

	public: // Methods:
		// Constructor/Destructor:
		EngineSet(uint32_t maxDirectionalLights, uint32_t maxPositionalLights);
		~EngineSet();

		// Non-copyable:
		EngineSet(const EngineSet& other) = delete;
		EngineSet& operator=(const EngineSet& other) = delete;

		// Movable:
		EngineSet(EngineSet&& other) noexcept;
		EngineSet& operator=(EngineSet&& other) noexcept;

		// Getters:
		const VkDescriptorSet& GetDescriptorSet();
	};
}