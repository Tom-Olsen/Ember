#pragma once
#include <vector>



// Forward decleration:
typedef struct VkPipeline_T* VkPipeline;
typedef struct VkPipelineLayout_T* VkPipelineLayout;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;
typedef struct VkShaderModule_T* VkShaderModule;

namespace vulkanRendererBackend
{
	/// <summary>
	/// Polymorphic parent class for all pipelines.
	/// </summary>
	class Pipeline
	{
	protected: // Members:
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkPipelineLayout m_pipelineLayout;
		VkPipeline m_pipeline;

	public: // Methods:
		Pipeline() = default;
		virtual ~Pipeline();

		// Non-copyable:
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		// Movable:
		Pipeline(Pipeline&& other) noexcept;
		Pipeline& operator=(Pipeline&& other) noexcept;

		const VkDescriptorSetLayout& GetVkDescriptorSetLayout() const;
		const VkPipelineLayout& GetVkPipelineLayout() const;
		const VkPipeline& GetVkPipeline() const;

	protected: // Methods:
		VkShaderModule CreateShaderModule(const std::vector<char>& code);

	private: // Methods;
		void Cleanup();
		void MoveFrom(Pipeline& other) noexcept;
	};
}