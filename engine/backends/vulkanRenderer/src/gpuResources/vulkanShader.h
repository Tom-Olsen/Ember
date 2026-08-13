#pragma once
#include "shaderReflection.h"
#include "vulkanRendererExport.h"
#include <filesystem>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Polymorphic parent class for Material and ComputeShader.
	/// </summary>
	class VULKAN_RENDERER_API Shader
	{
	protected: // Members:
		std::string m_debugName;
		emberSpirvReflect::ShaderReflection m_shaderReflection;
		std::vector<VkDescriptorSetLayout> m_vkDescriptorSetLayouts;
		VkPipelineLayout m_vkPipelineLayout;
	
	protected: // Methods:
		// Constructor:
		Shader(const std::string& debugName);

	public: // Methods:
		// Destructor:
		virtual ~Shader();

		// Non-copyable:
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		// Movable:
		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;

		// Create descriptor sets:
		void CreateDescriptorSetLayout();

		// Getters:
		const emberSpirvReflect::ShaderReflection& GetShaderReflection() const;
		const VkPipelineLayout& GetVkPipelineLayout() const;
		const std::vector<VkDescriptorSetLayout>& GetVkDescriptorSetLayout() const;
		const std::string& GetDebugName() const;

		// Debugging:
		void PrintShaderInfo() const;
	};
}