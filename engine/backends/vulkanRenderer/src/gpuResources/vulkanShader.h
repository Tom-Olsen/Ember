#pragma once
#include "shaderReflection.h"
#include "vulkanRendererExport.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Pipeline;
	class DescriptorSetBinding;



	/// <summary>
	/// Polymorphic parent class for Material and ComputeShader.
	/// </summary>
	class VULKAN_RENDERER_API Shader
	{
	protected: // Members:
		std::string m_name;
		std::vector<VkDescriptorSetLayout> m_vkDescriptorSetLayouts;
		VkPipelineLayout m_vkPipelineLayout;
		std::vector<std::unique_ptr<Pipeline>> m_pPipelines;
		emberSpirvReflect::ShaderReflection m_shaderReflection;
		std::unique_ptr<DescriptorSetBinding> m_pShaderDescriptorSetBinding;
	
	protected:  // Methods:
		// Constructor:
		Shader();

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
		const std::string& GetName() const;
		const emberSpirvReflect::ShaderReflection& const GetShaderReflection() const;
		const VkPipelineLayout& GetVkPipelineLayout() const;
		const DescriptorSetBinding* GetDescriptorSetBinding() const;

		// Debugging:
		void PrintShaderInfo() const;
	};
}